#ifndef DEEPREINFORCEMENTLEARNING_ALPHAZEROTRAININGTEMPLATE_H
#define DEEPREINFORCEMENTLEARNING_ALPHAZEROTRAININGTEMPLATE_H

#include <string>
#include <vector>
#include <time.h>
#include <cfloat>
#include <thread>
#include <mutex>
#include <RingBuffer.h>
#include "AlphaZeroUtility.h"
#include "NeuralNetworks/NeuralNetwork.h"
#include "MonteCarloTreeSearchTemplate.h"

using namespace ALZ;

struct AlphaZeroTrainingParameters
{
	std::string neuralNetPath;

	//Training Parameters
	bool TRAINING_DONT_USE_DRAWS = false;
	int CURRENT_ITERATION = 0;
	int SELFPLAY_BATCH_SIZE = 10;
	int TRAINING_ITERATIONS = 10000;
	int MIN_REPLAY_MEMORY_SIZE = 100;
	int SELF_PLAY_MCTS_COUNT = 100;
	int NUM_SELF_PLAY_GAMES = 100;
	int TRAINING_BATCH_SIZE = 100;
	int SAVE_ITERATION_COUNT = 10;
	int RANDOM_MOVE_COUNT = 15;
	int NUMBER_CPU_THREADS = 1;
	int MAX_REPLAY_MEMORY_SIZE = 40000;

	bool RESTRICT_GAME_LENGTH = true;
	int DRAW_AFTER_COUNT_OF_STEPS = 75;
};

template <typename GameStateT>
struct ReplayElementT
{
	ReplayElementT(GameStateT state, int currentPlayer, std::vector<std::pair<int, float>> probs, float result)
		: state(std::move(state))
		, currentPlayer(currentPlayer)
		, mctsProbabilities(std::move(probs))
		, result(result)
	{
	}

	GameStateT state;
	int currentPlayer;
	std::vector<std::pair<int, float>> mctsProbabilities;
	float result;
};

template <typename GameStateT, typename GameT, bool mockExpansion = false>
class AlphaZeroTrainingT
{
public:
	AlphaZeroTrainingT(GameT* game, NeuralNetwork* currentBest, torch::DeviceType device = torch::kCPU)
		: m_game(game)
		, m_net(currentBest)
		, m_actionCount(game->getActionCount())
		, m_device(device)
	{
	}

	void runTraining() 
	{
		if (m_params.CURRENT_ITERATION == 0)
			m_net->save(m_params.neuralNetPath + "/start");

		for (size_t iteration = m_params.CURRENT_ITERATION; iteration < (m_params.CURRENT_ITERATION + m_params.TRAINING_ITERATIONS); iteration++)
		{
			ALZ::ScopedTimer timer = ALZ::ScopedTimer("Iteration took: ");
			std::cout << "Current Iteration " << iteration << std::endl;
			selfPlay();
			trainNet();
			save(iteration);
		}
	}
	// Sets the training parameters and creates a new replay buffer -> all elements in replay buffer get destroyed
	void setTrainingParams(AlphaZeroTrainingParameters params) 
	{
		m_params = std::move(params);
		m_replayMemory = RingBuffer<ReplayElementT<GameStateT>>(m_params.MAX_REPLAY_MEMORY_SIZE);
	}

private:
	void selfPlay() 
	{
		m_gamesToPlay = m_params.NUM_SELF_PLAY_GAMES;
		m_net->setToEval();

		std::vector<std::thread> threadPool;
		for (size_t i = 0; i < m_params.NUMBER_CPU_THREADS; i++)
			threadPool.push_back(std::thread(&AlphaZeroTrainingT::selfPlayBuf, this));

		for (auto& thread : threadPool)
			thread.join();
	}

	void selfPlayBuf() 
	{
		int batchSize = 0;

		while (true)
		{
			{
				std::scoped_lock lock(m_mut);
				if (m_gamesToPlay <= 0)
					return;

				batchSize = m_params.SELFPLAY_BATCH_SIZE;
				m_gamesToPlay -= m_params.SELFPLAY_BATCH_SIZE;
				if (m_gamesToPlay < 0)
					batchSize += m_gamesToPlay;
			}

			auto resultVec = selfPlay(batchSize);

			std::scoped_lock lock(m_mut);
			m_replayMemory.add(std::move(resultVec));
			std::cout << m_gamesToPlay << std::endl;
		}
	}

	struct SelfPlayState
	{
		SelfPlayState(GameStateT currentState, int currentPlayer, int actionCount, torch::DeviceType device, GameT* game, MonteCarloTreeSearchCacheT<GameStateT, GameT, mockExpansion>* cache)
			: currentState(std::move(currentState))
			, currentPlayer(currentPlayer)
			, mcts(MonteCarloTreeSearchT<GameStateT, GameT, mockExpansion>(cache, game, device))
		{
		}

		GameStateT currentState;
		int currentPlayer;
		bool continueMcts = false;
		int netBufferIndex = -1;
		int currentStep = 0;
		MonteCarloTreeSearchT<GameStateT, GameT, mockExpansion> mcts;
		std::vector<ReplayElementT<GameStateT>> trainingData;
	};


	/*
	Previously for selfplay, the games were simulated sequentially: game1 -> move1, game1 -> move2 ... game2 -> move1, game2 -> move2
	It was possible to improve the performance by running multiple games in parallel,
	therefore collecting the data sent to the GPU and calculating multiple game states at once on the GPU.

	With this approach (down below) we calculate multiple games at once: game1 -> move1, game2 -> move1 ... game1 -> move2, game2 -> move2
	This has the benefit that we can collect the (neural net input) data for all games and calculate the neural net output once.
	The result is a big performance improvement, since GPUs are fast in calculating multiple data at once.
	*/
	std::vector<ReplayElementT<GameStateT>> selfPlay(int batchSize) 
	{
		assert(batchSize != 0);
		std::vector<ReplayElementT<GameStateT>> resultingTrainingsData;
		auto netInputBuffer = MonteCarloTreeSearchCacheT<GameStateT, GameT, mockExpansion>(m_device, m_game);
		auto currentStatesData = std::vector<SelfPlayState>(batchSize, { m_game->getInitialGameState(), m_game->getInitialPlayer(), m_game->getActionCount(), m_device, m_game, &netInputBuffer });
		/*
		Use a vector of pointers to the gamedata for iterating,
		then all of the game data can be destroyed at once.
		This should improve the performance quite a bit
		*/
		auto currentStates = std::vector<SelfPlayState*>();
		for (auto& elem : currentStatesData)
			currentStates.emplace_back(&elem);

		while (!currentStates.empty())
		{
			netInputBuffer.convertToNeuralInput();
			m_mut.lock();
			netInputBuffer.expand(m_net);
			m_mut.unlock();

			for (size_t i = 0; i < currentStates.size(); i++)
			{
				auto& currentStateObj = *(currentStates[i]);
				auto& mcts = currentStateObj.mcts;
				auto& currentState = currentStateObj.currentState;
				auto& continueMcts = currentStateObj.continueMcts;
				auto& currentPlayer = currentStateObj.currentPlayer;
				auto& netInputIndex = currentStateObj.netBufferIndex;
				auto& trainingData = currentStateObj.trainingData;
				auto& currentStep = currentStateObj.currentStep;

				if (m_params.RESTRICT_GAME_LENGTH && (currentStep >= m_params.DRAW_AFTER_COUNT_OF_STEPS))
				{
					addResult(trainingData, 0);
					if (!m_params.TRAINING_DONT_USE_DRAWS)
						merge(resultingTrainingsData, trainingData);

					currentStates.erase(currentStates.begin() + i); // Instead of erase it is thinkable to restart the game here
					i--;
					continue;
				}

				if (!continueMcts)
					continueMcts = mcts.startSearchWithoutExpansion(currentState, currentPlayer, m_params.SELF_PLAY_MCTS_COUNT);
				else
					continueMcts = mcts.expandAndContinueSearchWithoutExpansion(currentState, currentPlayer);

				if (continueMcts)
					continue;

				auto probs = mcts.getProbabilities(currentState);

				int action;
				if (currentStep < m_params.RANDOM_MOVE_COUNT)
					action = getRandomAction(probs);
				else
					action = getBestAction(probs);

				trainingData.emplace_back(currentState, currentPlayer, std::move(probs), -1);
				currentState = m_game->makeMove(currentState, action, currentPlayer);
				currentPlayer = m_game->getNextPlayer(currentPlayer);
				currentStep++;

				if (m_game->isGameOver(currentState))
				{
					addResult(trainingData, m_game->getPlayerWon(currentState));
					merge(resultingTrainingsData, trainingData);
					currentStates.erase(currentStates.begin() + i); // Instead of erase it is thinkable to restart the game here
					i--;
				}
			}
		}

		return resultingTrainingsData;
	}

	void addResult(std::vector<ReplayElementT<GameStateT>>& elements, int winner) 
	{
		for (auto& elem : elements)
		{
			int player = elem.currentPlayer;
			if (player == winner)
				elem.result = 1;
			else if (winner == 0)
				elem.result = 0;
			else
				elem.result = -1;
		}
	}

	void trainNet() 
	{
		constexpr int conversionCpuThreads = 8;
		m_net->setToTraining();
		std::cout << "Current Replay Memory Size " << m_replayMemory.size() << std::endl;
		if (m_replayMemory.size() < m_params.MIN_REPLAY_MEMORY_SIZE)
			return;

		m_trainingBatchIndex = 0;

		std::vector<std::thread> threadPool;
		for (size_t i = 0; i < conversionCpuThreads; i++)
			threadPool.push_back(std::thread(&AlphaZeroTrainingT::trainNetMultiThreaded, this));

		for (auto& thread : threadPool)
			thread.join();
	}

	void trainNetMultiThreaded() 
	{
		while (true)
		{
			{
				std::scoped_lock lock(m_mut);

				if (m_trainingBatchIndex >= (m_replayMemory.size() / m_params.TRAINING_BATCH_SIZE))
					return;

				m_trainingBatchIndex++;
			}

			std::vector<ReplayElementT<GameStateT>> batch = m_replayMemory.getRandomSample(m_params.TRAINING_BATCH_SIZE);
			torch::Tensor neuralInput = convertSampleToNeuralInput(batch);
			torch::Tensor valueTarget = convertToValueTarget(batch);
			torch::Tensor probsTarget = convertToProbsTarget(batch);

			{
				std::scoped_lock lock(m_mut);

				neuralInput = neuralInput.to(m_device);
				valueTarget = valueTarget.to(m_device);
				probsTarget = probsTarget.to(m_device);
				auto [valueTensor, probsTensor] = m_net->calculate(neuralInput);
				m_net->training(valueTensor, probsTensor, probsTarget, valueTarget);
			}
		}
	}

	torch::Tensor convertSampleToNeuralInput(const std::vector<ReplayElementT<GameStateT>>& sample)
	{
		const int sampleSize = sample.size();
		torch::Tensor neuralInput;

		bool declared = false;
		for (int i = 0; i < sampleSize; i++)
		{
			auto converted = m_game->convertStateToNeuralNetInput(sample[i].state, sample[i].currentPlayer);
			if (!declared)
			{
				int planes = converted.size(1);
				int width = converted.size(2);
				int height = converted.size(3);
				neuralInput = torch::zeros({ sampleSize, planes, width, height });
				declared = true;
			}
			neuralInput[i] = converted[0];
		}

		return neuralInput;
	}

	torch::Tensor convertToValueTarget(const std::vector<ReplayElementT<GameStateT>>& sample)
	{
		const int sampleSize = sample.size();
		torch::Tensor valueTarget = torch::zeros({ sampleSize, 1 });

		for (int i = 0; i < sampleSize; i++)
			valueTarget[i][0] = sample[i].result;

		return valueTarget;
	}

	torch::Tensor convertToProbsTarget(const std::vector<ReplayElementT<GameStateT>>& sample)
	{
		int sampleSize = sample.size();
		torch::Tensor probsTarget = torch::zeros({ sampleSize, m_actionCount });

		for (int x = 0; x < sampleSize; x++)
		{
			for (const auto& [action, prob] : sample[x].mctsProbabilities)
				probsTarget[x][action] = prob;
		}

		return probsTarget;
	}

	void save(int iteration) 
	{
		if ((iteration % m_params.SAVE_ITERATION_COUNT) == 0)
			m_net->save(m_params.neuralNetPath + "/iteration" + std::to_string(iteration));
	}

	GameT* m_game;
	NeuralNetwork* m_net;
	int m_actionCount = -1;
	torch::DeviceType m_device;
	int m_gamesToPlay = 0;
	size_t m_trainingBatchIndex = 0;
	std::mutex m_mut;
	AlphaZeroTrainingParameters m_params;
	RingBuffer<ReplayElementT<GameStateT>> m_replayMemory;
};

#endif //DEEPREINFORCEMENTLEARNING_ALPHAZEROTRAININGTEMPLATE_H

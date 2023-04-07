#include "AlphaZeroTraining.h"

using namespace ALZ;

AlphaZeroTraining::AlphaZeroTraining(int actionCount, NeuralNetwork* currentBest, torch::DeviceType device)
	: m_neuralNet(currentBest),
	m_actionCount(actionCount),
	m_device(device)
{
	m_replayMemory = RingBuffer<ReplayElement>(m_params.MAX_REPLAY_MEMORY_SIZE);
}

void AlphaZeroTraining::runTraining(Game* game)
{
	m_neuralNet->save(m_params.neuralNetPath + "/start");
	for (int iteration = 0; iteration < m_params.TRAINING_ITERATIONS; iteration++)
	{
		std::cout << "Current Iteration " << iteration << std::endl;
		selfPlayMultiThread(m_neuralNet, game);
		trainNet(m_neuralNet, game);
		save(iteration);
	}
}

void AlphaZeroTraining::selfPlayMultiThread(NeuralNetwork* net, Game* game)
{
	m_threadManager = std::make_unique<MultiThreadingNeuralNetManager>(m_params.NUMBER_CPU_THREADS, m_params.NUMBER_CPU_THREADS, net);
	std::vector<std::thread> threadPool;
	m_gamesToPlay = m_params.NUM_SELF_PLAY_GAMES;
	for (int i = 0; i < m_params.NUMBER_CPU_THREADS; i++)
		threadPool.push_back(std::thread(&AlphaZeroTraining::selfPlayMultiThreadGames, this, net, game, m_threadManager.get()));

	for (auto& thread : threadPool)
		thread.join();
}

void AlphaZeroTraining::selfPlayMultiThreadGames(NeuralNetwork* net, Game* game, MultiThreadingNeuralNetManager* threadManager)
{
	while (true)
	{
		m_mut.lock();
		if (m_gamesToPlay == 0)
		{
			threadManager->safeDecrementActiveThreads();
			m_mut.unlock();
			return;
		}
		m_gamesToPlay--;
		m_mut.unlock();

		std::vector<ReplayElement> trainData = selfPlayGame(net, game);

		m_mut.lock();
		m_replayMemory.add(std::move(trainData));
		m_mut.unlock();
	}
}

std::vector<ReplayElement> AlphaZeroTraining::selfPlayGame(NeuralNetwork* net, Game* game)
{
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(m_actionCount);

	std::vector<ReplayElement> trainingData;
	std::string currentState = game->getInitialGameState();
	int currentPlayer = game->getInitialPlayer();
	int currentStep = 0;
	bool gameTooLong = false;

	while (!game->isGameOver(currentState))
	{
		if (m_params.RESTRICT_GAME_LENGTH && (currentStep >= m_params.DRAW_AFTER_COUNT_OF_STEPS))
		{
			gameTooLong = true;
			break;
		}
		mcts.multiThreadedSearch(m_params.SELF_PLAY_MCTS_COUNT, currentState, game, currentPlayer, m_threadManager.get(), m_device);

		std::vector<float> probs = mcts.getProbabilities(currentState);

		int action;
		if (currentStep < m_params.RANDOM_MOVE_COUNT)
			action = getRandomIndex(probs, 1.0);
		else
			action = getMaxElementIndex(probs);

		trainingData.emplace_back(currentState, currentPlayer, std::move(probs), -1);
		currentState = game->makeMove(currentState, action, currentPlayer);
		currentPlayer = game->getNextPlayer(currentPlayer);
		currentStep++;
	}
	int playerWon = gameTooLong ? 0 : game->getPlayerWon(currentState);

	addResult(trainingData, playerWon);

	if (m_params.TRAINING_DONT_USE_DRAWS && (playerWon == 0))
		return {};

	return trainingData;
}

void AlphaZeroTraining::addResult(std::vector<ReplayElement>& elements, int winner)
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

void AlphaZeroTraining::trainNet(NeuralNetwork* net, Game* game)
{
	std::cout << "Current Replay Memory Size " << m_replayMemory.size() << std::endl;
	if (m_replayMemory.size() < m_params.MIN_REPLAY_MEMORY_SIZE)
		return;

	int batchIndex = 0;
	while (batchIndex < (m_replayMemory.size() / m_params.TRAINING_BATCH_SIZE))
	{
		std::vector<ReplayElement> batch = m_replayMemory.getRandomSample(m_params.TRAINING_BATCH_SIZE);

		torch::Tensor neuralInput = convertSampleToNeuralInput(batch, game);
		torch::Tensor valueTarget = convertToValueTarget(batch);
		torch::Tensor probsTarget = convertToProbsTarget(batch);
		auto [valueTensor, probsTensor] = net->calculate(neuralInput);

		net->training(valueTensor, probsTensor, probsTarget, valueTarget);

		batchIndex++;
	}
}

torch::Tensor AlphaZeroTraining::convertSampleToNeuralInput(const std::vector<ReplayElement>& sample, Game* game)
{
	int sampleSize = sample.size();
	torch::Tensor neuralInput;

	bool declared = false;
	for (int i = 0; i < sampleSize; i++)
	{
		auto converted = game->convertStateToNeuralNetInput(sample[i].state, sample[i].currentPlayer);
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
	neuralInput = neuralInput.to(m_device);

	return neuralInput;
}

torch::Tensor AlphaZeroTraining::convertToValueTarget(const std::vector<ReplayElement>& sample)
{
	int sampleSize = sample.size();
	torch::Tensor valueTarget = torch::zeros({ sampleSize, 1 });

	for (int i = 0; i < sampleSize; i++)
		valueTarget[i][0] = sample[i].result;

	valueTarget = valueTarget.to(m_device);

	return valueTarget;
}

torch::Tensor AlphaZeroTraining::convertToProbsTarget(const std::vector<ReplayElement>& sample)
{
	int sampleSize = sample.size();
	torch::Tensor probsTarget = torch::zeros({ sampleSize, m_actionCount });

	for (int x = 0; x < sampleSize; x++)
	{
		for (int y = 0; y < m_actionCount; y++)
		{
			probsTarget[x][y] = sample[x].mctsProbabilities[y];
		}
	}
	probsTarget = probsTarget.to(m_device);

	return probsTarget;
}

void AlphaZeroTraining::save(int iteration)
{
	if ((iteration % m_params.SAVE_ITERATION_COUNT) == 0)
		m_neuralNet->save(m_params.neuralNetPath + "/iteration" + std::to_string(iteration));
}

void AlphaZeroTraining::setTrainingParams(Parameters params)
{
	m_params = std::move(params);
	m_replayMemory = RingBuffer<ReplayElement>(m_params.MAX_REPLAY_MEMORY_SIZE);
}

namespace 
{
	struct GameState 
	{
		GameState(std::string currentState, int currentPlayer, int actionCount) 
			: currentState(currentState)
			, currentPlayer(currentPlayer)
			, mcts(MonteCarloTreeSearch(actionCount))
		{
		}

		std::string currentState;
		int currentPlayer;
		bool continueMcts = false;
		int netBufferIndex = -1;
		MonteCarloTreeSearch mcts;
		std::vector<ReplayElement> trainingData;
	};
}

std::vector<ReplayElement> AlphaZeroTraining::selfPlayBatch(NeuralNetwork* net, Game* game)
{
	std::vector<ReplayElement> resultingTrainingsData;
	bool gameTooLong = false;
	torch::DeviceType device = torch::kCUDA;
	NeuralNetInputBuffer netInputBuffer = {};
	constexpr int batchSize = 2;
	constexpr int mctsCount = 50;
	int finishedGamesCounter = 0;
	int currentStep = 0;
	auto currentStates = std::vector<GameState>(batchSize, { game->getInitialGameState(), game->getInitialPlayer(), game->getActionCount()});

	while (!currentStates.empty())
	{
		netInputBuffer.calculateOutput(net);

		for (size_t i = 0; i < currentStates.size(); i++)
		{
			auto& mcts = currentStates[i].mcts;
			auto& currentState = currentStates[i].currentState;
			auto& continueMcts = currentStates[i].continueMcts;
			auto& currentPlayer = currentStates[i].currentPlayer;
			auto& netInputIndex = currentStates[i].netBufferIndex;
			auto& trainingData = currentStates[i].trainingData;

			if (m_params.RESTRICT_GAME_LENGTH && (currentStep >= m_params.DRAW_AFTER_COUNT_OF_STEPS)) 
			{
				addResult(currentStates[i].trainingData, 0);
				if (!m_params.TRAINING_DONT_USE_DRAWS) 
				{
					resultingTrainingsData.insert(resultingTrainingsData.end()
						, std::make_move_iterator(trainingData.begin())
						, std::make_move_iterator(trainingData.end()));
				}

				currentStates.erase(currentStates.begin() + i); // Instead of erase it is thinkable to restart the game here
				i--;
				continue;
			}

			if (!continueMcts) 
			{
				continueMcts = mcts.specialSearch(currentState, game, currentPlayer, mctsCount);
			}
			else 
			{
				auto [valueTens, probTens] = netInputBuffer.getOutput(netInputIndex);
				continueMcts = mcts.continueSpecialSearch(currentState, game, currentPlayer, valueTens, probTens);
			}

			if (continueMcts)
			{
				netInputIndex = netInputBuffer.addToInput(mcts.getExpansionNeuralNetInput(game, device));
				continue;
			}

			std::vector<float> probs = mcts.getProbabilities(currentState);

			int action;
			if (currentStep < m_params.RANDOM_MOVE_COUNT)
				action = getRandomIndex(probs, 1.0);
			else
				action = getMaxElementIndex(probs);

			trainingData.emplace_back(currentState, currentPlayer, std::move(probs), -1);
			currentState = game->makeMove(currentState, action, currentPlayer);
			currentPlayer = game->getNextPlayer(currentPlayer);
			currentStep++;

			if (game->isGameOver(currentState)) 
			{
				addResult(trainingData, game->getPlayerWon(currentState));
				resultingTrainingsData.insert(resultingTrainingsData.end()
					, std::make_move_iterator(trainingData.begin())
					, std::make_move_iterator(trainingData.end()));
				currentStates.erase(currentStates.begin() + i); // Instead of erase it is thinkable to restart the game here
				i--;
			}
		}
	}

	return resultingTrainingsData;
}

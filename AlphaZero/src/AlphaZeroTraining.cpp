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
	if (m_params.CURRENT_ITERATION == 0)
		m_neuralNet->save(m_params.neuralNetPath + "/start");

	for (size_t iteration = m_params.CURRENT_ITERATION; iteration < (m_params.CURRENT_ITERATION + m_params.TRAINING_ITERATIONS); iteration++)
	{
		ScopedTimer timer = ScopedTimer("Iteration took: ");
		std::cout << "Current Iteration " << iteration << std::endl;
		selfPlay(m_neuralNet, game);
		trainNet(m_neuralNet, game);
		save(iteration);
	}
}

void AlphaZeroTraining::setTrainingParams(Parameters params)
{
	m_params = std::move(params);
	m_replayMemory = RingBuffer<ReplayElement>(m_params.MAX_REPLAY_MEMORY_SIZE);
}

void AlphaZeroTraining::selfPlay(NeuralNetwork* net, Game* game)
{
	m_gamesToPlay = m_params.NUM_SELF_PLAY_GAMES;
	net->setToEval();

	std::vector<std::thread> threadPool;
	for (size_t i = 0; i < m_params.NUMBER_CPU_THREADS; i++)
		threadPool.push_back(std::thread(&AlphaZeroTraining::selfPlayBuf, this, net, game));

	for (auto& thread : threadPool)
		thread.join();
}

void AlphaZeroTraining::selfPlayBuf(NeuralNetwork* net, Game* game)
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

		auto resultVec = selfPlay(net, game, batchSize);

		std::scoped_lock lock(m_mut);
		m_replayMemory.add(std::move(resultVec));
		std::cout << m_gamesToPlay << std::endl;
	}
}

namespace 
{
	struct GameState 
	{
		GameState(std::string currentState, int currentPlayer, int actionCount, torch::DeviceType device, Game* game, MonteCarloTreeSearchCache* cache)
			: currentState(currentState)
			, currentPlayer(currentPlayer)
			, mcts(MonteCarloTreeSearch(actionCount, device, game, cache))
		{
		}

		std::string currentState;
		int currentPlayer;
		bool continueMcts = false;
		int netBufferIndex = -1;
		int currentStep = 0;
		MonteCarloTreeSearch mcts;
		std::vector<ReplayElement> trainingData;
	};
}

/*
Previously for selfplay, the games were simulated sequentially: game1 -> move1, game1 -> move2 ... game2 -> move1, game2 -> move2
It was possible to improve the performance by running multiple games in parallel, 
therefore collecting the data sent to the GPU and calculating multiple game states at once on the GPU.

With this approach (down below) we calculate multiple games at once: game1 -> move1, game2 -> move1 ... game1 -> move2, game2 -> move2
This has the benefit that we can collect the (neural net input) data for all games and calculate the neural net output once.
The result is a big performance improvement, since GPUs are fast in calculating multiple data at once.
*/
std::vector<ReplayElement> AlphaZeroTraining::selfPlay(NeuralNetwork* net, Game* game, int batchSize)
{
	assert(batchSize != 0);
	std::vector<ReplayElement> resultingTrainingsData;
	auto netInputBuffer = MonteCarloTreeSearchCache(m_device, game);
	auto currentStatesData = std::vector<GameState>(batchSize, { game->getInitialGameState(), game->getInitialPlayer(), game->getActionCount(), m_device, game, &netInputBuffer});
	/*
	Use a vector of pointers to the gamedata for iterating,
	then all of the game data can be destroyed at once.
	This should improve the performance quite a bit
	*/
	auto currentStates = std::vector<GameState*>();
	for (auto& elem : currentStatesData) 
		currentStates.emplace_back(&elem);

	while (!currentStates.empty())
	{
		netInputBuffer.convertToNeuralInput();
		m_mut.lock();
		netInputBuffer.expand(net);
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
			currentState = game->makeMove(currentState, action, currentPlayer);
			currentPlayer = game->getNextPlayer(currentPlayer);
			currentStep++;

			if (game->isGameOver(currentState)) 
			{
				addResult(trainingData, game->getPlayerWon(currentState));
				merge(resultingTrainingsData, trainingData);
				currentStates.erase(currentStates.begin() + i); // Instead of erase it is thinkable to restart the game here
				i--;
			}
		}
	}

	return resultingTrainingsData;
}

void AlphaZeroTraining::printMemoryUsage(long long cacheMemSize, long long mctsMemsize, long long trainingsDataTotalSize) const
{
	std::cout << "Memory Usage in MB:" << std::endl;
	std::cout << "This batch MCTS: " << mctsMemsize / MegaByte << std::endl;
	std::cout << "This batch cache total: " << cacheMemSize / MegaByte << std::endl;
	std::cout << "Whole trainingsdata: " << trainingsDataTotalSize / MegaByte << std::endl;
	std::cout << "Total memsize (this batch + trainingsdata): " << (trainingsDataTotalSize + cacheMemSize + mctsMemsize) / MegaByte << std::endl << std::endl;
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
	constexpr int conversionCpuThreads = 8;
	net->setToTraining();
	std::cout << "Current Replay Memory Size " << m_replayMemory.size() << std::endl;
	if (m_replayMemory.size() < m_params.MIN_REPLAY_MEMORY_SIZE)
		return;

	m_trainingBatchIndex = 0;

	std::vector<std::thread> threadPool;
	for (size_t i = 0; i < conversionCpuThreads; i++)
		threadPool.push_back(std::thread(&AlphaZeroTraining::trainNetMultiThreaded, this, net, game));

	for (auto& thread : threadPool)
		thread.join();
}

// Only the generation of the neural net input and the target values is multi-threaded
void AlphaZeroTraining::trainNetMultiThreaded(NeuralNetwork* net, Game* game)
{
	while (true)
	{
		{
			std::scoped_lock lock(m_mut);

			if (m_trainingBatchIndex >= (m_replayMemory.size() / m_params.TRAINING_BATCH_SIZE)) 
				return;

			m_trainingBatchIndex++;
		}

		std::vector<ReplayElement> batch = m_replayMemory.getRandomSample(m_params.TRAINING_BATCH_SIZE);
		torch::Tensor neuralInput = convertSampleToNeuralInput(batch, game);
		torch::Tensor valueTarget = convertToValueTarget(batch);
		torch::Tensor probsTarget = convertToProbsTarget(batch);
		
		{
			std::scoped_lock lock(m_mut);

			neuralInput = neuralInput.to(m_device);
			valueTarget = valueTarget.to(m_device);
			probsTarget = probsTarget.to(m_device);
			auto [valueTensor, probsTensor] = net->calculate(neuralInput);
			net->training(valueTensor, probsTensor, probsTarget, valueTarget);
		}
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

	return neuralInput;
}

torch::Tensor AlphaZeroTraining::convertToValueTarget(const std::vector<ReplayElement>& sample)
{
	int sampleSize = sample.size();
	torch::Tensor valueTarget = torch::zeros({ sampleSize, 1 });

	for (int i = 0; i < sampleSize; i++)
		valueTarget[i][0] = sample[i].result;

	return valueTarget;
}

torch::Tensor AlphaZeroTraining::convertToProbsTarget(const std::vector<ReplayElement>& sample)
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

void AlphaZeroTraining::save(int iteration)
{
	if ((iteration % m_params.SAVE_ITERATION_COUNT) == 0)
		m_neuralNet->save(m_params.neuralNetPath + "/iteration" + std::to_string(iteration));
}
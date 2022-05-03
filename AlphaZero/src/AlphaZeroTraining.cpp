#include "AlphaZeroTraining.h"

using namespace ALZ;

AlphaZeroTraining::AlphaZeroTraining(int actionCount, NeuralNetwork* currentBest, torch::DeviceType device)
{
	this->neuralNet = currentBest;
	this->actionCount = actionCount;
	replayMemory = RingBuffer<ReplayElement>(MAX_REPLAY_MEMORY_SIZE);
	this->device = device;
}

void AlphaZeroTraining::runTraining(Game* game)
{
	neuralNet->save(neuralNetPath + "/start");
	for (int iteration = 0; iteration < TRAINING_ITERATIONS; iteration++)
	{
		std::cout << "Current Iteration " << iteration << std::endl;
		selfPlay(neuralNet, game);
		trainNet(neuralNet, game);
		save(iteration);
	}
}

void AlphaZeroTraining::selfPlay(NeuralNetwork* net, Game* game)
{
	if (NUMBER_CPU_THREADS <= 1)
		selfPlaySingleThread(net, game);
	else
		selfPlayMultiThread(net, game);
}

void AlphaZeroTraining::selfPlaySingleThread(NeuralNetwork* net, Game* game)
{
	for (int x = 0; x < NUM_SELF_PLAY_GAMES; x++)
		replayMemory.add(selfPlayGame(net, game, false));
}

void AlphaZeroTraining::selfPlayMultiThread(NeuralNetwork* net, Game* game)
{
	threadManager = std::make_unique<MultiThreadingNeuralNetManager>(NUMBER_CPU_THREADS, NUMBER_CPU_THREADS, net);
	std::vector<std::thread> threadPool;
	int gamesToPlay = NUM_SELF_PLAY_GAMES;
	for (int i = 0; i < NUMBER_CPU_THREADS; i++)
		threadPool.push_back(std::thread(&AlphaZeroTraining::selfPlayMultiThreadGames, this, net, game, std::ref(gamesToPlay), threadManager.get(), rand()));

	for (auto& thread : threadPool)
		thread.join();
}

void AlphaZeroTraining::selfPlayMultiThreadGames(NeuralNetwork* net, Game* game, int& gamesToPlay, MultiThreadingNeuralNetManager* threadManager, int seed)
{
	srand(seed);

	while (true)
	{
		mut.lock();
		if (gamesToPlay == 0)
		{
			threadManager->safeDecrementActiveThreads();
			mut.unlock();
			return;
		}
		else
		{
			gamesToPlay--;
			mut.unlock();
		}

		std::vector<ReplayElement> trainData = selfPlayGame(net, game, true);

		mut.lock();
		replayMemory.add(trainData);
		mut.unlock();
	}
}

std::vector<ReplayElement> AlphaZeroTraining::selfPlayGame(NeuralNetwork* net, Game* game, bool multiThreading)
{
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(actionCount);

	std::vector<ReplayElement> trainingData;
	std::string currentState = game->getInitialGameState();
	int currentPlayer = game->getInitialPlayer();
	int currentStep = 0;
	bool gameTooLong = false;

	while (!game->isGameOver(currentState))
	{
		if (RESTRICT_GAME_LENGTH)
		{
			if (currentStep != 0 && (currentStep % DRAW_AFTER_COUNT_OF_STEPS == 0))
			{
				gameTooLong = true;
				break;
			}
		}
		if (multiThreading)
			mcts.multiThreadedSearch(SELF_PLAY_MCTS_COUNT, currentState, game, currentPlayer, this->threadManager.get(), device);
		else
			mcts.search(SELF_PLAY_MCTS_COUNT, currentState, net, game, currentPlayer, device);
		std::vector<float> probs = mcts.getProbabilities(currentState);

		trainingData.emplace_back(currentState, currentPlayer, probs, -1);

		int action;
		if (currentStep < RANDOM_MOVE_COUNT)
			action = getRandomIndex(probs, 1.0);
		else
			action = getMaxElementIndex(probs);

		currentState = game->makeMove(currentState, action, currentPlayer);
		currentPlayer = game->getNextPlayer(currentPlayer);
		currentStep++;
	}
	int playerWon = 0;
	if (!gameTooLong)
		playerWon = game->getPlayerWon(currentState);

	addResult(trainingData, playerWon);

	if (TRAINING_DONT_USE_DRAWS && (playerWon == 0))
		trainingData.clear();

	return std::move(trainingData);
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
	std::cout << "Current Replay Memory Size " << replayMemory.size() << std::endl;
	if (replayMemory.size() < MIN_REPLAY_MEMORY_SIZE)
		return;

	int batchIndex = 0;
	while (batchIndex < (replayMemory.size() / TRAINING_BATCH_SIZE))
	{
		std::vector<ReplayElement> batch = replayMemory.getRandomSample(TRAINING_BATCH_SIZE);

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
	neuralInput = neuralInput.to(device);

	return neuralInput;
}

torch::Tensor AlphaZeroTraining::convertToValueTarget(const std::vector<ReplayElement>& sample)
{
	int sampleSize = sample.size();
	torch::Tensor valueTarget = torch::zeros({ sampleSize, 1 });

	for (int i = 0; i < sampleSize; i++)
		valueTarget[i][0] = sample[i].result;

	valueTarget = valueTarget.to(device);

	return valueTarget;
}

torch::Tensor AlphaZeroTraining::convertToProbsTarget(const std::vector<ReplayElement>& sample)
{
	int sampleSize = sample.size();
	torch::Tensor probsTarget = torch::zeros({ sampleSize, actionCount });

	for (int x = 0; x < sampleSize; x++)
	{
		for (int y = 0; y < actionCount; y++)
		{
			probsTarget[x][y] = sample[x].mctsProbabilities[y];
		}
	}
	probsTarget = probsTarget.to(device);

	return probsTarget;
}

void AlphaZeroTraining::save(int iteration)
{
	if ((iteration % SAVE_ITERATION_COUNT) == 0)
		neuralNet->save(neuralNetPath + "/iteration" + std::to_string(iteration));
}

void AlphaZeroTraining::setMaxReplayMemorySize(int size)
{
	this->MAX_REPLAY_MEMORY_SIZE = size;
	replayMemory = RingBuffer<ReplayElement>(MAX_REPLAY_MEMORY_SIZE);
}

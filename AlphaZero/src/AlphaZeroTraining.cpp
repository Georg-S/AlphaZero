#include "AlphaZeroTraining.h"

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
	for (int iteration = 0; iteration < TRAINING_ITERATIONS; iteration++) {
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
	omp_lock_t writelock;
	omp_init_lock(&writelock);
	omp_set_num_threads(NUMBER_CPU_THREADS);
#pragma omp parallel for
	for (int x = 0; x < NUM_SELF_PLAY_GAMES; x++) {
		std::vector<ReplayElement> trainData = selfPlayGame(net, game, false);

		omp_set_lock(&writelock);
		replayMemory.add(trainData);
		omp_unset_lock(&writelock);
	}
}

void AlphaZeroTraining::selfPlayMultiThread(NeuralNetwork* net, Game* game)
{
	threadManager = std::make_unique<MultiThreadingNeuralNetManager>(NUMBER_CPU_THREADS, NUMBER_CPU_THREADS, net);
	std::vector<std::thread> threadPool;
	int gamesToPlay = NUM_SELF_PLAY_GAMES;
	for (int i = 0; i < NUMBER_CPU_THREADS; i++)
		threadPool.push_back(std::thread(&AlphaZeroTraining::selfPlayMultiThreadGames, this, net, game, std::ref(gamesToPlay), threadManager.get()));

	for (auto& thread : threadPool)
		thread.join();
}

void AlphaZeroTraining::selfPlayMultiThreadGames(NeuralNetwork* net, Game* game, int& gamesToPlay, MultiThreadingNeuralNetManager* threadManager)
{
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

	while (!game->isGameOver(currentState)) {
		if (RESTRICT_GAME_LENGTH) {
			if (currentStep != 0 && (currentStep % DRAW_AFTER_COUNT_OF_STEPS == 0)) {
				gameTooLong = true;
				break;
			}
		}
		if (multiThreading)
			mcts.multiThreadedSearch(SELF_PLAY_MCTS_COUNT, currentState, net, game, currentPlayer, this->threadManager.get(), device);
		else
			mcts.search(SELF_PLAY_MCTS_COUNT, currentState, net, game, currentPlayer, device);
		std::vector<float> probs = mcts.getProbabilities(currentState);

		trainingData.push_back(ReplayElement(currentState, currentPlayer, probs, -1));

		int action;
		if (currentStep < RANDOM_MOVE_COUNT)
			action = getRandomAction(probs);
		else
			action = getArgMaxIndex(probs);

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

	return trainingData;
}

int AlphaZeroTraining::getRandomAction(const std::vector<float>& probabilities)
{
	float r = ((float)rand() / (RAND_MAX));
	if (r == 0)
		r += FLT_MIN;
	else if (r == 1)
		r -= FLT_MIN;
	float acc = 0.f;
	int x = 0;
	for (; x < probabilities.size(); x++) {
		acc += probabilities[x];

		if (acc >= r)
			return x;
	}

	return x;
}

void AlphaZeroTraining::addResult(std::vector<ReplayElement>& elements, int winner)
{
	for (int i = 0; i < elements.size(); i++) {
		int player = elements[i].currentPlayer;
		if (player == winner)
			elements[i].result = 1;
		else if (winner == 0)
			elements[i].result = 0;
		else
			elements[i].result = -1;
	}
}

void AlphaZeroTraining::trainNet(NeuralNetwork* net, Game* game)
{
	std::cout << "Current Replay Memory Size " << replayMemory.size() << std::endl;
	if (replayMemory.size() < MIN_REPLAY_MEMORY_SIZE)
		return;

	int batchIndex = 0;
	while (batchIndex < (replayMemory.size() / TRAINING_BATCH_SIZE)) {
		std::vector<ReplayElement> batch;
		replayMemory.getRandomSample(TRAINING_BATCH_SIZE, batch);

		torch::Tensor neuralInput = convertSampleToNeuralInput(batch, game);
		torch::Tensor valueTarget = convertToValueTarget(batch);
		torch::Tensor probsTarget = convertToProbsTarget(batch);
		auto buf = net->calculate(neuralInput);
		torch::Tensor probsTensor = std::get<1>(buf);
		torch::Tensor valueTensor = std::get<0>(buf);

		net->training(valueTensor, probsTensor, probsTarget, valueTarget);

		batchIndex++;
	}
}

torch::Tensor AlphaZeroTraining::convertSampleToNeuralInput(const std::vector<ReplayElement>& sample, Game* game)
{
	int sampleSize = sample.size();
	torch::Tensor neuralInput;

	bool declared = false;
	for (int i = 0; i < sampleSize; i++) {
		auto converted = game->convertStateToNeuralNetInput(sample[i].state, sample[i].currentPlayer);
		if (!declared) {
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

	for (int x = 0; x < sampleSize; x++) {
		for (int y = 0; y < actionCount; y++) {
			probsTarget[x][y] = sample[x].mctsProbabilities[y];
		}
	}
	probsTarget = probsTarget.to(device);

	return probsTarget;
}

int AlphaZeroTraining::getArgMaxIndex(const std::vector<float>& vec)
{
	return std::max_element(vec.begin(), vec.end()) - vec.begin();
}

void AlphaZeroTraining::save(int iteration)
{
	if ((iteration % SAVE_ITERATION_COUNT) == 0)
		neuralNet->save(neuralNetPath + "/iteration" + std::to_string(iteration));
}

void AlphaZeroTraining::setMaxReplayMemorySize(int size)
{
	this->MAX_REPLAY_MEMORY_SIZE = size;

	replayMemory.clear();
	replayMemory = RingBuffer<ReplayElement>(MAX_REPLAY_MEMORY_SIZE);
}

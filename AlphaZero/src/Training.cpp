#include "Training.h"

Training::Training(int actionCount, NeuralNetwork* net) {
	this->currentBest = net;
	net->save("NeuralNets/initial");
	this->apprentice = net;
	this->actionCount = actionCount;
	replayMemory = RingBuffer<ReplayElement>(MAX_REPLAY_MEMORY_SIZE);
}

Training::Training(int actionCount, NeuralNetwork* currentBest, NeuralNetwork* apprentice, torch::DeviceType device) {
	this->currentBest = currentBest;
	this->apprentice = apprentice;
	this->actionCount = actionCount;
	replayMemory = RingBuffer<ReplayElement>(MAX_REPLAY_MEMORY_SIZE);
	this->device = device;
}

void Training::runTraining(Game* game) {
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(actionCount);
	for (int iteration = 0; iteration < TRAINING_ITERATIONS; iteration++) {
		std::cout << "Current Iteration " << iteration << std::endl;
		selfPlay(currentBest, game, mcts);
		apprentice->save("NeuralNets/apprenticeBuf");
		trainNet(apprentice, game);
		evaluation(game, iteration);
	}
}

void Training::selfPlay(NeuralNetwork* net, Game* game, MonteCarloTreeSearch mcts) {
	for (int x = 0; x < NUM_SELF_PLAY_GAMES; x++) {
		mcts.clearAll();
		std::vector<ReplayElement> trainData = selfPlayGame(net, game, mcts);
		replayMemory.add(trainData);
	}
}

std::vector<ReplayElement> Training::selfPlayGame(NeuralNetwork* net, Game* game, MonteCarloTreeSearch mcts) {
	std::vector<ReplayElement> trainingData;
	std::string currentState = game->getInitialGameState();
	int currentPlayer = game->getInitialPlayer();
	int currentStep = 0;

	while (!game->isGameOver(currentState)) {
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
	int playerWon = game->getPlayerWon(currentState);
	addResult(trainingData, playerWon);

	if (TRAINING_DONT_USE_DRAWS && (playerWon == 0))
		trainingData.clear();

	return trainingData;
}

int Training::getRandomAction(const std::vector<float>& probabilities) {
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

void Training::addResult(std::vector<ReplayElement>& elements, int winner) {
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

void Training::trainNet(NeuralNetwork* net, Game* game) {
	std::cout << "Current Replay Memory Size " << replayMemory.size() << std::endl;
	if (replayMemory.size() < MIN_REPLAY_MEMORY_SIZE)
		return;


	int batchIndex = 0;
	while (batchIndex < (replayMemory.size() / TRAINING_BATCH_SIZE)) {
		//    for(int i = 0; i < TRAINING_ROUND_COUNT; i++) {
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

torch::Tensor Training::convertSampleToNeuralInput(const std::vector<ReplayElement>& sample, Game* game) {
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

torch::Tensor Training::convertToValueTarget(const std::vector<ReplayElement>& sample) {
	int sampleSize = sample.size();
	torch::Tensor valueTarget = torch::zeros({ sampleSize, 1 });

	for (int i = 0; i < sampleSize; i++)
		valueTarget[i][0] = sample[i].result;

	valueTarget = valueTarget.to(device);

	return valueTarget;
}

torch::Tensor Training::convertToProbsTarget(const std::vector<ReplayElement>& sample) {
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

void Training::evaluation(Game* game, int iteration) {
	if (iteration % EVALUATION_STEP != 0)
		return;

	float score = evaluateApprentice(game);
	if (score > EVALUATION_THRESHOLD) {
		currentBest->copyNetFrom(apprentice);
		save(iteration);
		std::cout << "Improved on Iteration " << iteration << std::endl;

		if (CLEAR_REPLAY_MEMORY_AFTER_IMPROVEMENT)
			replayMemory.clear();
	}
	else {
		if (LOAD_OLD_NET_IF_NOT_IMPROVED)
			apprentice->load("NeuralNets/apprenticeBuf");
	}
}

float Training::evaluateApprentice(Game* game) {
	float apprenticeScore = 0.f;
	int apprenticePlayer = game->getInitialPlayer();

	MonteCarloTreeSearch mctsApprentice = MonteCarloTreeSearch(actionCount);
	MonteCarloTreeSearch mctsCurrentBest = MonteCarloTreeSearch(actionCount);


	for (int i = 0; i < EVALUATION_GAMES; i++) {
		mctsApprentice.clearAll();
		mctsCurrentBest.clearAll();

		int winner = 0;

		if (apprenticePlayer == 1)
			winner = determinsticSelfPlayGame(apprentice, currentBest, game, mctsApprentice, mctsCurrentBest);
		else
			winner = determinsticSelfPlayGame(currentBest, apprentice, game, mctsCurrentBest, mctsApprentice);

		if (winner == apprenticePlayer)
			apprenticeScore += 1.f;
		else if (winner == 0)
			apprenticeScore += 0.5;

		apprenticePlayer = game->getNextPlayer(apprenticePlayer);
	}
	apprenticeScore /= EVALUATION_GAMES;
	std::cout << apprenticeScore << std::endl;

	return apprenticeScore;
}

int Training::determinsticSelfPlayGame(NeuralNetwork* first, NeuralNetwork* second, Game* game, MonteCarloTreeSearch& mctsFirst, MonteCarloTreeSearch& mctsSecond) {
	std::string state = game->getInitialGameState();
	int currentPlayer = game->getInitialPlayer();
	MonteCarloTreeSearch* currentMcts = &mctsFirst;
	NeuralNetwork* currentNet = first;

	while (!game->isGameOver(state)) {
		currentMcts->search(EVALUATION_MCTS_COUNT, state, currentNet, game, currentPlayer, device);
		std::vector<float> probs = currentMcts->getProbabilities(state);
		int action = getArgMaxIndex(probs);
		state = game->makeMove(state, action, currentPlayer);
		currentPlayer = game->getNextPlayer(currentPlayer);

		if (currentMcts == &mctsFirst) {
			currentMcts = &mctsSecond;
			currentNet = second;
		}
		else {
			currentMcts = &mctsFirst;
			currentNet = first;
		}
	}
	mctsFirst.clearAll();
	mctsSecond.clearAll();

	return game->getPlayerWon(state);
}

int Training::getArgMaxIndex(const std::vector<float>& vec) {
	return std::max_element(vec.begin(), vec.end()) - vec.begin();
}

void Training::save(const int& iteration) {
	currentBest->save("NeuralNets/improved" + std::to_string(iteration));
}

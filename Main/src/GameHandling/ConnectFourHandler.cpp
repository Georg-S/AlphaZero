#include "GameHandling/ConnectFourHandler.h"

ConnectFourHandler::ConnectFourHandler() {
}

ConnectFourHandler::~ConnectFourHandler() {
}

void ConnectFourHandler::connectFourAgainstNeuralNetAi(cn4::PlayerColor playerColor, std::string netName, int countMcts, bool probabilistic, torch::DeviceType device) {
	ConnectFourAdapter adap = ConnectFourAdapter();
	DefaultNoResidual neuralNet = DefaultNoResidual(2, 7, 6, 7, preTrainedPath + "/" + netName, device);
	NeuralNetAi* ai = new NeuralNetAi(&neuralNet, &adap, 7, countMcts, probabilistic, device);
	cn4::PlayerColor aiColor = (cn4::PlayerColor)(playerColor % 2 + 1);
	ConnectFour connectFour = ConnectFour(aiColor, ai);

	connectFour.gameLoop();
}

void ConnectFourHandler::connectFourAgainstMinMaxAi(int depth, cn4::PlayerColor color) {
	cn4::MinMaxAi ai = cn4::MinMaxAi(depth);
	cn4::PlayerColor  aiColor = (cn4::PlayerColor)(color % 2 + 1);
	ConnectFour connect = ConnectFour(aiColor, &ai);
	connect.gameLoop();
}

void ConnectFourHandler::startTwoPlayerConnectFourGame() {
	ConnectFour connect = ConnectFour();
	connect.gameLoop();
}

void ConnectFourHandler::traininingPerformanceTest(torch::DeviceType device) {
	ConnectFourAdapter adap = ConnectFourAdapter();
	DefaultNoResidual* neuralNet = new DefaultNoResidual(2, 7, 6, 7, device);
	AlphaZeroTraining training = AlphaZeroTraining(7, neuralNet, device);
	loadPerformanceTestParameters(training);


	uint64_t before = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	training.runTraining(&adap);
	int64_t after = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	std::cout << (after - before) / 1000.f << std::endl;
}

void ConnectFourHandler::loadDefaultParametersForAlphaZeroTraining(AlphaZeroTraining& connectFourZero) {
	connectFourZero.setMaxReplayMemorySize(100000);
	connectFourZero.neuralNetPath = trainingPath;
	connectFourZero.TRAINING_DONT_USE_DRAWS = false;
	connectFourZero.RESTRICT_GAME_LENGTH = false;

	connectFourZero.DRAW_AFTER_COUNT_OF_STEPS = 50;

	connectFourZero.TRAINING_ITERATIONS = 10000;
	connectFourZero.MIN_REPLAY_MEMORY_SIZE = 100;
	connectFourZero.SELF_PLAY_MCTS_COUNT = mctsCount;
	connectFourZero.NUM_SELF_PLAY_GAMES = 100;
	connectFourZero.TRAINING_BATCH_SIZE = 100;
	connectFourZero.SAVE_ITERATION_COUNT = 1;
	connectFourZero.RANDOM_MOVE_COUNT = 10;
}

void ConnectFourHandler::loadPerformanceTestParameters(AlphaZeroTraining& connectFourZero) {
	loadDefaultParametersForAlphaZeroTraining(connectFourZero);
	connectFourZero.TRAINING_ITERATIONS = 1;
	connectFourZero.NUM_SELF_PLAY_GAMES = 1;
}

void ConnectFourHandler::evalConnectFour() {
	std::ofstream myfile;
	myfile.open(std::to_string(evalMCTSCount) + "_100_200k_001.csv");

	myfile << "Iteration; Wins; Draws; Losses \n";


	int minMaxDepth = 0;

	EvalResult result = evalConnectFour(preTrainedPath + "/start", minMaxDepth, torch::kCUDA);
	writeEvaluationResultToFile(0, result, myfile);

	for (int i = 0; i < 30; i++) {
		std::string path = preTrainedPath + "/iteration" + std::to_string(i);
		std::cout << path << std::endl;
		EvalResult result = evalConnectFour(path, minMaxDepth, torch::kCUDA);
		writeEvaluationResultToFile(i + 1, result, myfile);
	}

	myfile.close();
}

void ConnectFourHandler::writeEvaluationResultToFile(int iteration, const EvalResult& result, std::ofstream& file) {
	file << std::to_string(iteration) << ";" << std::to_string(result.wins) << ";" << std::to_string(result.draws) << ";" << std::to_string(result.losses) << std::endl;
}

EvalResult ConnectFourHandler::evalConnectFour(std::string netName, int minMaxDepth, torch::DeviceType device) {
	ConnectFourAdapter adap = ConnectFourAdapter();
	DefaultNoResidual* toEval = new DefaultNoResidual(2, 7, 6, 7, netName, device);
	NeuralNetAi neuralNetAi = NeuralNetAi(toEval, &adap, 7, evalMCTSCount, false, device);
	cn4::MinMaxAi ai1 = cn4::MinMaxAi(minMaxDepth);

	EvalResult result = Evaluation::eval(&neuralNetAi, &ai1, &adap, 50);
	delete toEval;
	return result;
}

void ConnectFourHandler::setTrainingParameters(AlphaZeroTraining& training, const TrainingParameters& params) {
	training.setMaxReplayMemorySize(params.replayMemorySize);
	training.neuralNetPath = trainingPath;
	training.TRAINING_DONT_USE_DRAWS = !params.useDraws;
	training.RESTRICT_GAME_LENGTH = params.restrictGameLength;

	training.DRAW_AFTER_COUNT_OF_STEPS = params.maxGameLength;

	training.TRAINING_ITERATIONS = params.trainingIterations;
	training.SELF_PLAY_MCTS_COUNT = params.selfPlayMctsCount;
	training.NUM_SELF_PLAY_GAMES = params.selfPlayGamesCount;
	training.TRAINING_BATCH_SIZE = params.trainingBatchSize;
	training.SAVE_ITERATION_COUNT = params.saveIterationCount;
	training.RANDOM_MOVE_COUNT = params.randomizedMoveCount;
}

void ConnectFourHandler::runTrainingWithDefaultParameters() {
	ConnectFourAdapter adap = ConnectFourAdapter();
	torch::DeviceType device = torch::kCUDA;
	DefaultNoResidual* neuralNet = new DefaultNoResidual(2, 7, 6, 7, device);
	AlphaZeroTraining training = AlphaZeroTraining(7, neuralNet, device);
	loadDefaultParametersForAlphaZeroTraining(training);

	training.runTraining(&adap);
}

void ConnectFourHandler::runTraining(const TrainingParameters& params) {
	ConnectFourAdapter adap = ConnectFourAdapter();
	torch::DeviceType device = params.device;
	DefaultNoResidual* neuralNet = new DefaultNoResidual(2, 7, 6, 7, device);
	neuralNet->setLearningRate(params.learningRate);
	AlphaZeroTraining training = AlphaZeroTraining(7, neuralNet, device);
	setTrainingParameters(training, params);

	training.runTraining(&adap);
}

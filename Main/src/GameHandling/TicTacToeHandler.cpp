#include "GameHandling/TicTacToeHandler.h"

TicTacToeHandler::TicTacToeHandler() {
	adap = TicTacToeAdapter();
}

TicTacToeHandler::~TicTacToeHandler() {

}

void TicTacToeHandler::ticTacToeAgainstMiniMaxAi(int playerColor) {
	ttt::MiniMaxAi ai = ttt::MiniMaxAi();
	int aiColor = ttt::GameLogic::getNextPlayer(playerColor);
	ttt::MiniMaxAi miMaxAi = ttt::MiniMaxAi();
	TicTacToe tic = TicTacToe(&miMaxAi, aiColor);
	tic.gameLoop();
}

void TicTacToeHandler::runTrainingWithAlphaZero(torch::DeviceType device) {
	DefaultNeuralNet* neuralNet = new DefaultNeuralNet(2, 3, 3, 9, device);
	AlphaZeroTraining training = AlphaZeroTraining(9, neuralNet, device);
	loadDefaultParametersForAlphaZeroTraining(training);

	training.runTraining(&adap);
}

void TicTacToeHandler::startTwoPlayerTicTacToeGame() {
	TicTacToe ttt = TicTacToe();
	ttt.gameLoop();
}

void TicTacToeHandler::traininingPerformanceTest(torch::DeviceType device) {
	DefaultNeuralNet* neuralNet = new DefaultNeuralNet(2, 3, 3, 9, device);
	AlphaZeroTraining training = AlphaZeroTraining(9, neuralNet, device);

	loadPerformanceTestParameters(training);

	uint64_t before = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	training.runTraining(&adap);
	int64_t after = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	float difference = (after - before) / 1000.F;
	std::cout << "Time passed: " << difference << std::endl;
}

void TicTacToeHandler::evalTicTacToe() {

	std::ofstream myfile;
	myfile.open(std::to_string(evalMCTSCount) + "_50_100k_001.csv");
	myfile << "Iteration; Wins; Draws; Losses \n";

	EvalResult result = evalTicTacToe(preTrainedPath + "/start", torch::kCUDA);
	writeEvaluationResultToFile(0, result, myfile);

	for (int i = 0; i < 40; i++) {
		std::string path = preTrainedPath + "/iteration" + std::to_string(i);
		std::cout << path << std::endl;
		EvalResult result = evalTicTacToe(path, torch::kCUDA);
		writeEvaluationResultToFile(i + 1, result, myfile);
	}


	myfile.close();
}

EvalResult TicTacToeHandler::evalTicTacToe(std::string netName, torch::DeviceType device) {
	DefaultNeuralNet* toEval = new DefaultNeuralNet(2, 3, 3, 9, netName, device);
	NeuralNetAi neuralNetAi = NeuralNetAi(toEval, &adap, 9, evalMCTSCount, false, device);
	ttt::MiniMaxAi minimaxAi = ttt::MiniMaxAi();
	EvalResult result = Evaluation::eval(&neuralNetAi, &minimaxAi, &adap);
	delete toEval;
	return result;
}

void TicTacToeHandler::writeEvaluationResultToFile(int iteration, const EvalResult& result, std::ofstream& file) {
	file << std::to_string(iteration) << ";" << std::to_string(result.wins) << ";" << std::to_string(result.draws) << ";" << std::to_string(result.losses) << std::endl;
}

void TicTacToeHandler::loadDefaultParametersForAlphaZeroTraining(AlphaZeroTraining& ticTacToeZero) {
	ticTacToeZero.neuralNetPath = trainingPath;
	ticTacToeZero.TRAINING_DONT_USE_DRAWS = false;
	ticTacToeZero.RESTRICT_GAME_LENGTH = false;

	ticTacToeZero.DRAW_AFTER_COUNT_OF_STEPS = 50;
	ticTacToeZero.TRAINING_ITERATIONS = 10000;
	ticTacToeZero.setMaxReplayMemorySize(40000);
	ticTacToeZero.MIN_REPLAY_MEMORY_SIZE = 100;
	ticTacToeZero.SELF_PLAY_MCTS_COUNT = mcts_count;
	ticTacToeZero.NUM_SELF_PLAY_GAMES = 1000;
	ticTacToeZero.TRAINING_BATCH_SIZE = 100;
	ticTacToeZero.SAVE_ITERATION_COUNT = 1;
	ticTacToeZero.RANDOM_MOVE_COUNT = 3;
}

void TicTacToeHandler::loadPerformanceTestParameters(AlphaZeroTraining& ticTacToeZero) {
	loadDefaultParametersForAlphaZeroTraining(ticTacToeZero);
	ticTacToeZero.NUM_SELF_PLAY_GAMES = 20;
	ticTacToeZero.TRAINING_ITERATIONS = 1;
}

void TicTacToeHandler::runTraining(const TrainingParameters& params) {
	TicTacToeAdapter adap = TicTacToeAdapter();

	torch::DeviceType device = params.device;
	DefaultNeuralNet* neuralNet = new DefaultNeuralNet(2, 3, 3, 9, device);
	neuralNet->setLearningRate(params.learningRate);
	AlphaZeroTraining training = AlphaZeroTraining(9, neuralNet, device);
	setTrainingParameters(training, params);

	training.runTraining(&adap);
}

void TicTacToeHandler::setTrainingParameters(AlphaZeroTraining& training, const TrainingParameters& params) {
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

void
TicTacToeHandler::ticTacToeAgainstNeuralNetAi(int playerColor, std::string netName, int countMcts, bool probabilistic,
	torch::DeviceType device) {
	TicTacToeAdapter adap = TicTacToeAdapter();
	DefaultNeuralNet neuralNet = DefaultNeuralNet(2, 3, 3, 9, preTrainedPath + "/" + netName, device);
	NeuralNetAi ai = NeuralNetAi(&neuralNet, &adap, 9, countMcts, probabilistic, device);
	int aiColor = ttt::GameLogic::getNextPlayer(playerColor);
	TicTacToe ttt = TicTacToe(&ai, aiColor);

	ttt.gameLoop();
}

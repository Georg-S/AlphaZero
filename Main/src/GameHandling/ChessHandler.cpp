#include "GameHandling/ChessHandler.h"

ChessHandler::ChessHandler()
{

}

void ChessHandler::runTrainingWithDefaultParameters(torch::DeviceType device)
{
	DefaultNeuralNet* chessNet = new DefaultNeuralNet(12, 8, 8, 4096, device);
	chessNet->setLearningRate(0.2);
	ReducedChessAdapter adap = ReducedChessAdapter();
	AlphaZeroTraining alphaZero = AlphaZeroTraining(4096, chessNet, device);
	loadDefaultParametersForAlphaZeroTraining(alphaZero);

	alphaZero.runTraining(&adap);
}

void ChessHandler::chessAgainstNeuralNetAi(chess::PieceColor playerColor, std::string netName, int mctsCount, bool randomize,
	torch::DeviceType device)
{
	DefaultNeuralNet* chessNet = new DefaultNeuralNet(12, 8, 8, 4096, preTrainedPath + "/" + netName, device);
	ReducedChessAdapter adap = ReducedChessAdapter();
	NeuralNetAi neuralNetAi = NeuralNetAi(chessNet, &adap, 4096, mctsCount, randomize, device);
	Chess chess = Chess(playerColor, &neuralNetAi);

	chess.gameLoop();
}

void ChessHandler::startTwoPlayerChessGame() 
{
	Chess chess = Chess();
	chess.gameLoop();
}

void ChessHandler::traininingPerformanceTest(torch::DeviceType device) 
{
	DefaultNeuralNet* chessNet = new DefaultNeuralNet(12, 8, 8, 4096, device);
	ReducedChessAdapter adap = ReducedChessAdapter();
	AlphaZeroTraining alphaZero = AlphaZeroTraining(4096, chessNet, device);
	loadPerformanceTestParameters(alphaZero);

	uint64_t before = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	alphaZero.runTraining(&adap);
	int64_t after = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	std::cout << (after - before) / 1000.f << std::endl;
}

void ChessHandler::loadDefaultParametersForAlphaZeroTraining(AlphaZeroTraining& chessZero) 
{
	chessZero.setMaxReplayMemorySize(300000);
	chessZero.neuralNetPath = trainingPath;
	chessZero.TRAINING_DONT_USE_DRAWS = false;
	chessZero.RESTRICT_GAME_LENGTH = true;

	chessZero.DRAW_AFTER_COUNT_OF_STEPS = 100;
	chessZero.TRAINING_ITERATIONS = 10000;
	chessZero.MIN_REPLAY_MEMORY_SIZE = 100;
	chessZero.SELF_PLAY_MCTS_COUNT = mctsCount;
	chessZero.NUM_SELF_PLAY_GAMES = 1000;
	chessZero.TRAINING_BATCH_SIZE = 100;
	chessZero.SAVE_ITERATION_COUNT = 1;
	chessZero.RANDOM_MOVE_COUNT = 20;
}

void ChessHandler::loadPerformanceTestParameters(AlphaZeroTraining& chessZero) 
{
	loadDefaultParametersForAlphaZeroTraining(chessZero);

	chessZero.TRAINING_ITERATIONS = 1;
	chessZero.NUM_SELF_PLAY_GAMES = 1;
	chessZero.SELF_PLAY_MCTS_COUNT = 800;
}

void ChessHandler::setTrainingParameters(AlphaZeroTraining& training, const TrainingParameters& params) 
{
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
	training.NUMBER_CPU_THREADS = params.cpuThreads;
}

void ChessHandler::runTraining(const TrainingParameters& params) 
{
	ReducedChessAdapter adap = ReducedChessAdapter();
	torch::DeviceType device = params.device;
	DefaultNeuralNet* neuralNet = new DefaultNeuralNet(12, 8, 8, 4096, device);
	neuralNet->setLearningRate(params.learningRate);
	AlphaZeroTraining training = AlphaZeroTraining(4096, neuralNet, device);
	setTrainingParameters(training, params);

	training.runTraining(&adap);
	delete neuralNet;
}

void ChessHandler::chessAgainstMiniMaxAi(int miniMaxDepth, chess::PieceColor playerColor) 
{
	chess::MiniMaxAi miniMax = chess::MiniMaxAi(miniMaxDepth);
	Chess chess = Chess(playerColor, &miniMax);

	chess.gameLoop();
}

#include "GameHandling/ChessHandler.h"

void ChessHandler::chessAgainstNeuralNetAi(ceg::PieceColor playerColor, std::string netName, int mctsCount, bool randomize,
	torch::DeviceType device)
{
	auto chessNet = std::make_unique<DefaultNeuralNet>(14, 8, 8, 4096, preTrainedPath + "/" + netName, device);
	chessNet->setToEval();
	ChessAdapter adap = ChessAdapter();
	NeuralNetAi neuralNetAi = NeuralNetAi(chessNet.get(), &adap, 4096, mctsCount, randomize, device);
	Chess chess = Chess(&neuralNetAi, playerColor);

	chess.game_loop();
}

void ChessHandler::startTwoPlayerChessGame()
{
	Chess chess = Chess();
	chess.game_loop();
}

AlphaZeroTraining::Parameters ChessHandler::getDefaultChessTrainingParameters() const
{
	auto params = AlphaZeroTraining::Parameters{};
	params.MAX_REPLAY_MEMORY_SIZE = 300000;
	params.neuralNetPath = trainingPath;
	params.TRAINING_DONT_USE_DRAWS = false;
	params.RESTRICT_GAME_LENGTH = true;

	params.DRAW_AFTER_COUNT_OF_STEPS = 100;
	params.TRAINING_ITERATIONS = 10000;
	params.MIN_REPLAY_MEMORY_SIZE = 100;
	params.SELF_PLAY_MCTS_COUNT = mctsCount;
	params.NUM_SELF_PLAY_GAMES = 1000;
	params.TRAINING_BATCH_SIZE = 100;
	params.SAVE_ITERATION_COUNT = 1;
	params.RANDOM_MOVE_COUNT = 20;
	params.SELFPLAY_BATCH_SIZE = 10;

	return params;
}

void ChessHandler::setTrainingParameters(AlphaZeroTraining& training, const TrainingParameters& params)
{
	auto defaultParams = getDefaultChessTrainingParameters();
	auto trainingParams = params.getAlphaZeroParams(trainingPath, defaultParams);
	training.setTrainingParams(trainingParams);
}

void ChessHandler::runTraining(const TrainingParameters& params)
{
	ChessAdapter adap = ChessAdapter();
	torch::DeviceType device = params.device;
	auto neuralNet = std::make_unique<DefaultNeuralNet>(14, 8, 8, 4096, device);
	neuralNet->setLearningRate(params.learningRate);
	neuralNet->setToTraining();
	AlphaZeroTraining training = AlphaZeroTraining(4096, neuralNet.get(), device);
	setTrainingParameters(training, params);

	ALZ::ScopedTimer timer{};
	training.runTraining(&adap);
}

void ChessHandler::chessAgainstMiniMaxAi(int miniMaxDepth, ceg::PieceColor playerColor)
{
	auto miniMaxAi = chess::NegaMaxAi(miniMaxDepth);
	Chess chess = Chess(&miniMaxAi, playerColor);

	chess.game_loop();
}

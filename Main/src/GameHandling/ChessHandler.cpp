#include "GameHandling/ChessHandler.h"

void ChessHandler::chessAgainstNeuralNetAi(ceg::PieceColor playerColor, std::string netName, int mctsCount, bool randomize,
	torch::DeviceType device)
{
	ChessAdapter adap = ChessAdapter();
	auto chessNet = std::make_unique<DefaultNeuralNet>(14, 8, 8, 4096, preTrainedPath + "/" + netName, device);
	chessNet->setToEval();
	auto cache = MonteCarloTreeSearchCache<ChessAdapter::GameState, ChessAdapter>(device, &adap);
	auto neuralNetAi = NeuralNetAi<ChessAdapter::GameState, ChessAdapter, false>(chessNet.get(), &adap, mctsCount, randomize, device);
	Chess chess = Chess(&neuralNetAi, playerColor);

	chess.game_loop();
}

void ChessHandler::startTwoPlayerChessGame()
{
	Chess chess = Chess();
	chess.game_loop();
}

AlphaZeroTrainingParameters ChessHandler::getDefaultChessTrainingParameters() const
{
	auto params = AlphaZeroTrainingParameters{};
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

void ChessHandler::setTrainingParameters(AlphaZeroTraining<ChessAdapter::GameState, ChessAdapter>& training, const TrainingParameters& params, int currentIteration)
{
	auto defaultParams = getDefaultChessTrainingParameters();
	auto trainingParams = params.getAlphaZeroParams(trainingPath, defaultParams);
	trainingParams.CURRENT_ITERATION = currentIteration;
	training.setTrainingParams(trainingParams);
}

static std::pair<std::string, int> getHighestExistingNetAndIteration(const std::string& path) 
{
	std::string res = "";
	int highestExistingNet = -1;
	std::smatch baseMatch;
	for (const auto& entry : std::filesystem::directory_iterator(path)) 
	{
		auto pathString = entry.path().string();
		if (std::regex_search(pathString, baseMatch, std::regex("\\d+")))
		{
			int iteration = std::stoi(baseMatch[0]);
			if (iteration > highestExistingNet) 
			{
				res = pathString;
				highestExistingNet = iteration;
			}
		}
	}

	return { res, highestExistingNet };
}

void ChessHandler::runTraining(const TrainingParameters& params)
{
	ChessAdapter adap = ChessAdapter();
	torch::DeviceType device = params.device;
	std::unique_ptr<DefaultNeuralNet> neuralNet = nullptr;
	const auto [netName, highestIteration] = getHighestExistingNetAndIteration(trainingPath);
	if (highestIteration == -1)
	{
		neuralNet = std::make_unique<DefaultNeuralNet>(14, 8, 8, 4096, device);
	}
	else 
	{
		std::cout << "Continue training with net: " << netName << std::endl;
		neuralNet = std::make_unique<DefaultNeuralNet>(14, 8, 8, 4096, netName, device);
	}
	int currentIteration = highestIteration + 1;

	neuralNet->setLearningRate(params.learningRate);
	neuralNet->setToTraining();
	auto training = AlphaZeroTraining<ChessAdapter::GameState, ChessAdapter>(&adap, neuralNet.get(), device);
	setTrainingParameters(training, params, currentIteration);

	ALZ::ScopedTimer timer{};
	training.runTraining();
}

void ChessHandler::chessAgainstMiniMaxAi(int miniMaxDepth, ceg::PieceColor playerColor)
{
	auto miniMaxAi = chess::NegaMaxAi(miniMaxDepth);
	Chess chess = Chess(&miniMaxAi, playerColor);

	chess.game_loop();
}

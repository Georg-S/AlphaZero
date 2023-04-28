#include "GameHandling/TicTacToeHandler.h"

using namespace ttt;

void TicTacToeHandler::ticTacToeAgainstMiniMaxAi(PlayerColor playerColor)
{
	PlayerColor aiColor = getNextPlayer(playerColor);
	MiniMaxAi miMaxAi = MiniMaxAi();
	TicTacToe tic = TicTacToe(&miMaxAi, aiColor);
	tic.gameLoop();
}

void TicTacToeHandler::startTwoPlayerTicTacToeGame()
{
	TicTacToe ttt = TicTacToe();
	ttt.gameLoop();
}

void TicTacToeHandler::evalTicTacToe(bool multiThreaded)
{
	constexpr int multiThreadingThreadCount = 10;
	std::ofstream myfile;
	myfile.open(std::to_string(evalMCTSCount) + "_50_100k_001.csv");
	myfile << "Iteration; Wins; Draws; Losses \n";
	EvalResult result;

	const int threads = multiThreaded ? multiThreadingThreadCount : 1;

	result = evalTicTacToeMultiThreaded(preTrainedPath + "/start", torch::kCUDA, threads);

	writeEvaluationResultToFile(0, result, myfile);

	for (int i = 0; i < 40; i++)
	{
		std::string path = preTrainedPath + "/iteration" + std::to_string(i);
		std::cout << path << std::endl;
		result = evalTicTacToeMultiThreaded(path, torch::kCUDA, threads);

		writeEvaluationResultToFile(i + 1, result, myfile);
	}
}

EvalResult TicTacToeHandler::evalTicTacToeMultiThreaded(std::string netName, torch::DeviceType device, int threadCount)
{
	TicTacToeAdapter adap = TicTacToeAdapter();
	Evaluation evaluation = Evaluation(device, evalMCTSCount);

	auto toEval = std::make_unique<DefaultNeuralNet>(2, 3, 3, 9, netName, device);
	toEval->setToEval();
	ttt::MiniMaxAi minimaxAi = ttt::MiniMaxAi();
	EvalResult result = {};
	evaluation.eval(toEval.get(), &minimaxAi, &adap,  100, result, 100);

	return result;
}

void TicTacToeHandler::writeEvaluationResultToFile(int iteration, const EvalResult& result, std::ofstream& file)
{
	file << std::to_string(iteration) << ";" << std::to_string(result.wins) << ";" << std::to_string(result.draws) << ";" << std::to_string(result.losses) << std::endl;
}

AlphaZeroTraining::Parameters TicTacToeHandler::getDefaultTicTacToeTrainingParameters() const
{
	auto params = AlphaZeroTraining::Parameters{};
	params.neuralNetPath = trainingPath;
	params.TRAINING_DONT_USE_DRAWS = false;
	params.RESTRICT_GAME_LENGTH = false;
	params.DRAW_AFTER_COUNT_OF_STEPS = 50;
	params.TRAINING_ITERATIONS = 10000;
	params.MAX_REPLAY_MEMORY_SIZE = 40000;
	params.MIN_REPLAY_MEMORY_SIZE = 100;
	params.SELF_PLAY_MCTS_COUNT = mcts_count;
	params.NUM_SELF_PLAY_GAMES = 1000;
	params.TRAINING_BATCH_SIZE = 100;
	params.SAVE_ITERATION_COUNT = 1;
	params.RANDOM_MOVE_COUNT = 3;
	params.SELFPLAY_BATCH_SIZE = 1000;

	return params;
}

void TicTacToeHandler::runTraining(const TrainingParameters& params)
{
	TicTacToeAdapter adap = TicTacToeAdapter();

	torch::DeviceType device = params.device;
	auto neuralNet = std::make_unique<DefaultNeuralNet>(2, 3, 3, 9, device);
	neuralNet->setLearningRate(params.learningRate);
	neuralNet->setToTraining();
	AlphaZeroTraining training = AlphaZeroTraining(9, neuralNet.get(), device);
	setTrainingParameters(training, params);

	ALZ::ScopedTimer timer{};
	training.runTraining(&adap);
}

void TicTacToeHandler::setTrainingParameters(AlphaZeroTraining& training, const TrainingParameters& params)
{
	auto defaultParams = getDefaultTicTacToeTrainingParameters();
	auto trainingParams = params.getAlphaZeroParams(trainingPath, defaultParams);
	training.setTrainingParams(trainingParams);
}

void TicTacToeHandler::ticTacToeAgainstNeuralNetAi(ttt::PlayerColor playerColor, std::string netName, int countMcts, bool probabilistic,
	torch::DeviceType device)
{
	TicTacToeAdapter adap = TicTacToeAdapter();
	DefaultNeuralNet neuralNet = DefaultNeuralNet(2, 3, 3, 9, preTrainedPath + "/" + netName, device);
	neuralNet.setToEval();
	NeuralNetAi ai = NeuralNetAi(&neuralNet, &adap, 9, countMcts, probabilistic, device);
	PlayerColor aiColor = getNextPlayer(playerColor);
	TicTacToe ttt = TicTacToe(&ai, aiColor);

	ttt.gameLoop();
}

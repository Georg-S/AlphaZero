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

void TicTacToeHandler::traininingPerformanceTest(torch::DeviceType device)
{
	TicTacToeAdapter adap = TicTacToeAdapter();
	auto neuralNet = std::make_unique<DefaultNeuralNet>(2, 3, 3, 9, device);
	AlphaZeroTraining training = AlphaZeroTraining(9, neuralNet.get(), device);

	loadPerformanceTestParameters(training);

	uint64_t before = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	training.runTraining(&adap);
	int64_t after = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	float difference = (after - before) / 1000.F;
	std::cout << "Time passed: " << difference << std::endl;
}

void TicTacToeHandler::evalTicTacToe(bool multiThreaded)
{
	std::ofstream myfile;
	myfile.open(std::to_string(evalMCTSCount) + "_50_100k_001.csv");
	myfile << "Iteration; Wins; Draws; Losses \n";
	EvalResult result;

	if (multiThreaded)
		result = evalTicTacToeMultiThreaded(preTrainedPath + "/start", torch::kCUDA);
	else
		result = evalTicTacToe(preTrainedPath + "/start", torch::kCUDA);
	writeEvaluationResultToFile(0, result, myfile);

	for (int i = 0; i < 10; i++)
	{
		std::string path = preTrainedPath + "/iteration" + std::to_string(i);
		std::cout << path << std::endl;
		if (multiThreaded)
			result = evalTicTacToeMultiThreaded(path, torch::kCUDA);
		else
			result = evalTicTacToe(path, torch::kCUDA);
		writeEvaluationResultToFile(i + 1, result, myfile);
	}
}

EvalResult TicTacToeHandler::evalTicTacToe(std::string netName, torch::DeviceType device)
{
	TicTacToeAdapter adap = TicTacToeAdapter();
	auto toEval = std::make_unique <DefaultNeuralNet>(2, 3, 3, 9, netName, device);
	NeuralNetAi neuralNetAi = NeuralNetAi(toEval.get(), &adap, 9, evalMCTSCount, false, device);
	ttt::MiniMaxAi minimaxAi = ttt::MiniMaxAi();
	EvalResult result = Evaluation::eval(&neuralNetAi, &minimaxAi, &adap);

	return result;
}

EvalResult TicTacToeHandler::evalTicTacToeMultiThreaded(std::string netName, torch::DeviceType device)
{
	TicTacToeAdapter adap = TicTacToeAdapter();
	constexpr int threadCount = 5;

	Evaluation evaluation = Evaluation(torch::kCUDA, evalMCTSCount);

	auto toEval = std::make_unique<DefaultNeuralNet>(2, 3, 3, 9, netName, device);
	MultiThreadingNeuralNetManager threadManager = MultiThreadingNeuralNetManager(threadCount, threadCount, toEval.get());
	ttt::MiniMaxAi minimaxAi = ttt::MiniMaxAi();
	EvalResult result = evaluation.evalMultiThreaded(&threadManager, &minimaxAi, &adap);

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

	return params;
}

void TicTacToeHandler::loadPerformanceTestParameters(AlphaZeroTraining& ticTacToeZero)
{
	auto params = getDefaultTicTacToeTrainingParameters();
	params.NUM_SELF_PLAY_GAMES = 20;
	params.TRAINING_ITERATIONS = 1;
	ticTacToeZero.setTrainingParams(params);
}

void TicTacToeHandler::runTraining(const TrainingParameters& params)
{
	TicTacToeAdapter adap = TicTacToeAdapter();

	torch::DeviceType device = params.device;
	auto neuralNet = std::make_unique<DefaultNeuralNet>(2, 3, 3, 9, device);
	neuralNet->setLearningRate(params.learningRate);
	AlphaZeroTraining training = AlphaZeroTraining(9, neuralNet.get(), device);
	setTrainingParameters(training, params);

	training.runTraining(&adap);
}

void TicTacToeHandler::setTrainingParameters(AlphaZeroTraining& training, const TrainingParameters& params)
{
	auto trainingParams = params.getAlphaZeroParams(trainingPath);
	training.setTrainingParams(trainingParams);
}

void TicTacToeHandler::ticTacToeAgainstNeuralNetAi(ttt::PlayerColor playerColor, std::string netName, int countMcts, bool probabilistic,
	torch::DeviceType device)
{
	TicTacToeAdapter adap = TicTacToeAdapter();
	DefaultNeuralNet neuralNet = DefaultNeuralNet(2, 3, 3, 9, preTrainedPath + "/" + netName, device);
	NeuralNetAi ai = NeuralNetAi(&neuralNet, &adap, 9, countMcts, probabilistic, device);
	PlayerColor aiColor = getNextPlayer(playerColor);
	TicTacToe ttt = TicTacToe(&ai, aiColor);

	ttt.gameLoop();
}

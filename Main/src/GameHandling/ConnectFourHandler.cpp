#include "GameHandling/ConnectFourHandler.h"

void ConnectFourHandler::connectFourAgainstNeuralNetAi(cn4::PlayerColor playerColor, std::string netName,
	int countMcts, bool probabilistic, torch::DeviceType device)
{
	ConnectFourAdapter adap = ConnectFourAdapter();
	auto neuralNet = std::make_unique<DefaultNeuralNet>(2, 7, 6, 7, preTrainedPath + "/" + netName, device);
	neuralNet->setToEval();
	NeuralNetAi* ai = new NeuralNetAi(neuralNet.get(), &adap, 7, countMcts, probabilistic, device);
	cn4::PlayerColor aiColor = (cn4::PlayerColor)((int)playerColor % 2 + 1);
	ConnectFour connectFour = ConnectFour(aiColor, ai);

	connectFour.gameLoop();
}

void ConnectFourHandler::connectFourAgainstMiniMaxAi(int depth, cn4::PlayerColor color)
{
	cn4::NegaMaxAi ai = cn4::NegaMaxAi(depth);
	cn4::PlayerColor  aiColor = (cn4::PlayerColor)((int)color % 2 + 1);
	ConnectFour connect = ConnectFour(aiColor, &ai);
	connect.gameLoop();
}

void ConnectFourHandler::startTwoPlayerConnectFourGame()
{
	ConnectFour connect = ConnectFour();
	connect.gameLoop();
}

void ConnectFourHandler::traininingPerformanceTest(torch::DeviceType device)
{
	ConnectFourAdapter adap = ConnectFourAdapter();
	auto neuralNet = std::make_unique<DefaultNeuralNet>(2, 7, 6, 7, device);
	neuralNet->setToTraining();
	AlphaZeroTraining training = AlphaZeroTraining(7, neuralNet.get(), device);
	loadPerformanceTestParameters(training);


	uint64_t before = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	training.runTraining(&adap);
	int64_t after = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	std::cout << (after - before) / 1000.f << std::endl;
}

void ConnectFourHandler::loadPerformanceTestParameters(AlphaZeroTraining& connectFourZero)
{
	auto params = getDefaultConnectFourTrainingParameters();
	params.TRAINING_ITERATIONS = 1;
	params.NUM_SELF_PLAY_GAMES = 1;
	connectFourZero.setTrainingParams(params);
}

void ConnectFourHandler::evalConnectFour(bool multiThreaded)
{
	constexpr int multiThreadingThreads = 10;
	EvalResult result;
	std::ofstream myfile;
	myfile.open(std::to_string(evalMCTSCount) + "_100_200k_001.csv");

	myfile << "Iteration; Wins; Draws; Losses \n";

	const int miniMaxDepth = 5;
	const int threads = multiThreaded ? multiThreadingThreads : 1;

	result = evalConnectFourMultiThreaded(preTrainedPath + "/start", miniMaxDepth, torch::kCUDA, threads);

	writeEvaluationResultToFile(0, result, myfile);

	for (int i = 0; i < 25; i++)
	{
		std::string path = preTrainedPath + "/iteration" + std::to_string(i);
		std::cout << path << std::endl;

		result = evalConnectFourMultiThreaded(path, miniMaxDepth, torch::kCUDA, threads);

		writeEvaluationResultToFile(i + 1, result, myfile);
	}

	myfile.close();
}

void ConnectFourHandler::writeEvaluationResultToFile(int iteration, const EvalResult& result, std::ofstream& file)
{
	file << std::to_string(iteration) << ";" << std::to_string(result.wins) << ";" << std::to_string(result.draws) << ";" << std::to_string(result.losses) << std::endl;
}

AlphaZeroTraining::Parameters ConnectFourHandler::getDefaultConnectFourTrainingParameters() const
{
	auto params = AlphaZeroTraining::Parameters{};
	params.MAX_REPLAY_MEMORY_SIZE = 100000;
	params.neuralNetPath = trainingPath;
	params.TRAINING_DONT_USE_DRAWS = false;
	params.RESTRICT_GAME_LENGTH = false;
	params.DRAW_AFTER_COUNT_OF_STEPS = 50;
	params.TRAINING_ITERATIONS = 10000;
	params.MIN_REPLAY_MEMORY_SIZE = 100;
	params.SELF_PLAY_MCTS_COUNT = mctsCount;
	params.NUM_SELF_PLAY_GAMES = 100;
	params.TRAINING_BATCH_SIZE = 100;
	params.SAVE_ITERATION_COUNT = 1;
	params.RANDOM_MOVE_COUNT = 10;

	return params;
}

EvalResult ConnectFourHandler::evalConnectFourMultiThreaded(std::string netName, int miniMaxDepth, torch::DeviceType device, int threadCount)
{
	Evaluation evaluation = Evaluation(torch::kCUDA, evalMCTSCount);
	auto toEval = std::make_unique<DefaultNeuralNet>(2, 7, 6, 7, netName, device);
	toEval->setToEval();
	MultiThreadingNeuralNetManager threadManager = MultiThreadingNeuralNetManager(threadCount, threadCount, toEval.get());
	cn4::NegaMaxAi miniMaxAi = cn4::NegaMaxAi(miniMaxDepth);
	ConnectFourAdapter adap = ConnectFourAdapter();
	EvalResult result = evaluation.evalMultiThreaded(&threadManager, &miniMaxAi, &adap);

	return result;
}

void ConnectFourHandler::setTrainingParameters(AlphaZeroTraining& training, const TrainingParameters& params)
{
	auto trainingParams = params.getAlphaZeroParams(trainingPath);
	training.setTrainingParams(trainingParams);
}

void ConnectFourHandler::runTraining(const TrainingParameters& params)
{
	ConnectFourAdapter adap = ConnectFourAdapter();
	torch::DeviceType device = params.device;
	auto neuralNet = std::make_unique<DefaultNeuralNet>(2, 7, 6, 7, device);
	neuralNet->setLearningRate(params.learningRate);
	neuralNet->setToTraining();
	AlphaZeroTraining training = AlphaZeroTraining(7, neuralNet.get(), device);
	setTrainingParameters(training, params);

	ALZ::ScopedTimer timer{};
	training.runTraining(&adap);
}

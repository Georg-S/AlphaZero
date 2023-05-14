#include "GameHandling/ConnectFourHandler.h"

void ConnectFourHandler::connectFourAgainstNeuralNetAi(cn4::PlayerColor playerColor, std::string netName,
	int countMcts, bool probabilistic, torch::DeviceType device)
{
	ConnectFourAdapter adap = ConnectFourAdapter();
	auto neuralNet = std::make_unique<DefaultNeuralNet>(2, 7, 6, 7, preTrainedPath + "/" + netName, device);
	neuralNet->setToEval();
	auto ai = NeuralNetAi<cn4::Board, ConnectFourAdapter, false>(neuralNet.get(), &adap, countMcts, probabilistic, device);
	cn4::PlayerColor aiColor = cn4::PlayerColor(adap.getNextPlayer(static_cast<int>(playerColor)));
	ConnectFour connectFour = ConnectFour(aiColor, &ai);

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



AlphaZeroTrainingParameters ConnectFourHandler::getDefaultConnectFourTrainingParameters() const
{
	auto params = AlphaZeroTrainingParameters{};
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
	params.SELFPLAY_BATCH_SIZE = 100;

	return params;
}

void ConnectFourHandler::setTrainingParameters(AlphaZeroTraining<cn4::Board, ConnectFourAdapter>& training, const TrainingParameters& params)
{
	auto defaultParams = getDefaultConnectFourTrainingParameters();
	auto trainingParams = params.getAlphaZeroParams(trainingPath, defaultParams);
	training.setTrainingParams(trainingParams);
}

void ConnectFourHandler::runTraining(const TrainingParameters& params)
{
	ConnectFourAdapter adap = ConnectFourAdapter();
	torch::DeviceType device = params.device;
	auto neuralNet = std::make_unique<DefaultNeuralNet>(2, 7, 6, 7, device);
	neuralNet->setLearningRate(params.learningRate);
	neuralNet->setToTraining();
	auto training = AlphaZeroTraining<cn4::Board, ConnectFourAdapter>(&adap, neuralNet.get(), device);
	setTrainingParameters(training, params);

	ALZ::ScopedTimer timer{};
	training.runTraining();
}

void ConnectFourHandler::evalConnectFour()
{
	const int miniMaxDepth = 5;
	EvalResult result;
	std::ofstream myfile;

	myfile.open(std::to_string(evalMCTSCount) + "_100_200k_001.csv");
	myfile << "Iteration; Wins; Draws; Losses \n";

	result = evalConnectFour(preTrainedPath + "/start", miniMaxDepth, torch::kCUDA);
	writeEvaluationResultToFile(0, result, myfile);

	for (int i = 0; i < 40; i++)
	{
		std::string path = preTrainedPath + "/iteration" + std::to_string(i);
		std::cout << path << std::endl;

		result = evalConnectFour(path, miniMaxDepth, torch::kCUDA);

		writeEvaluationResultToFile(i + 1, result, myfile);
	}

	myfile.close();
}

EvalResult ConnectFourHandler::evalConnectFour(std::string netName, int miniMaxDepth, torch::DeviceType device)
{
	ConnectFourAdapter adap = ConnectFourAdapter();
	Evaluation evaluation = Evaluation<cn4::Board, ConnectFourAdapter, false>(device, evalMCTSCount, &adap);
	auto toEval = std::make_unique<DefaultNeuralNet>(2, 7, 6, 7, netName, device);
	toEval->setToEval();
	cn4::NegaMaxAi miniMaxAi = cn4::NegaMaxAi(miniMaxDepth);
	EvalResult result = {};
	evaluation.eval(toEval.get(), &miniMaxAi, 100, result, 100);

	return result;
}

void ConnectFourHandler::writeEvaluationResultToFile(int iteration, const EvalResult& result, std::ofstream& file)
{
	file << std::to_string(iteration) << ";" << std::to_string(result.wins) << ";" << std::to_string(result.draws) << ";" << std::to_string(result.losses) << std::endl;
}

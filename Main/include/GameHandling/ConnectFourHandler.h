#ifndef DEEPREINFORCEMENTLEARNING_CONNECTFOURHANDLER_H
#define DEEPREINFORCEMENTLEARNING_CONNECTFOURHANDLER_H

#include <iostream>
#include <string>
#include <chrono>
#include <fstream>
#include <AlphaZeroTraining.h>
#include <NeuralNetworks/DefaultNeuralNet.h>
#include <ConnectFour/ConnectFourAdapter.h>
#include <ConnectFour/ConnectFour.h>
#include <ConnectFour/MiniMaxAi.h>
#include <AI/NeuralNetAi.h>
#include "Evaluation.h"
#include "TrainingParameters.h"


class ConnectFourHandler {
public:
	ConnectFourHandler();
	~ConnectFourHandler();


	void connectFourAgainstNeuralNetAi(cn4::PlayerColor playerColor, std::string netName, int countMcts, bool probabilistic, torch::DeviceType device);
	void connectFourAgainstMiniMaxAi(int depth, cn4::PlayerColor playerColor);

	void runTrainingWithDefaultParameters();
	void runTraining(const TrainingParameters& params);
	void startTwoPlayerConnectFourGame();
	void traininingPerformanceTest(torch::DeviceType device);
	void evalConnectFour();
	void setTrainingParameters(AlphaZeroTraining& training, const TrainingParameters& params);

private:
	EvalResult evalConnectFour(std::string netName, int miniMaxDepth, torch::DeviceType device = torch::kCPU);
	void writeEvaluationResultToFile(int iteration, const EvalResult& result, std::ofstream& file);
	void loadDefaultParametersForAlphaZeroTraining(AlphaZeroTraining& connectFourZero);
	void loadPerformanceTestParameters(AlphaZeroTraining& connectFourZero);

	std::string trainingPath = "NeuralNets/Training/ConnectFour";
	std::string preTrainedPath = "NeuralNets/PreTrained/ConnectFour";
	int mctsCount = 100;


	int evalMCTSCount = 100;
};


#endif //DEEPREINFORCEMENTLEARNING_CONNECTFOURHANDLER_H

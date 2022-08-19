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
#include <ConnectFour/NegaMaxAi.h>
#include <Other/NeuralNetAi.h>
#include "Evaluation.h"
#include "TrainingParameters.h"


class ConnectFourHandler
{
public:
	ConnectFourHandler() = default;
	void connectFourAgainstNeuralNetAi(cn4::PlayerColor playerColor, std::string netName, int countMcts, bool probabilistic, torch::DeviceType device);
	void connectFourAgainstMiniMaxAi(int depth, cn4::PlayerColor playerColor);
	void runTraining(const TrainingParameters& params);
	void startTwoPlayerConnectFourGame();
	void traininingPerformanceTest(torch::DeviceType device);
	void evalConnectFour(bool multiThreaded = true);
	void setTrainingParameters(AlphaZeroTraining& training, const TrainingParameters& params);

private:
	EvalResult evalConnectFourMultiThreaded(std::string netName, int miniMaxDepth, torch::DeviceType device, int threadCount);
	void writeEvaluationResultToFile(int iteration, const EvalResult& result, std::ofstream& file);
	AlphaZeroTraining::Parameters getDefaultConnectFourTrainingParameters() const;
	void loadPerformanceTestParameters(AlphaZeroTraining& connectFourZero);

	inline static const std::string trainingPath = "NeuralNets/Training/ConnectFour";
	inline static const std::string preTrainedPath = "NeuralNets/PreTrained/ConnectFour";
	int mctsCount = 100;
	int evalMCTSCount = 100;
};


#endif //DEEPREINFORCEMENTLEARNING_CONNECTFOURHANDLER_H

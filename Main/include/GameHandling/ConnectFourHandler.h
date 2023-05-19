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
#include "TrainingUtility.h"


class ConnectFourHandler
{
public:
	ConnectFourHandler() = default;
	void connectFourAgainstNeuralNetAi(cn4::PlayerColor playerColor, std::string netName, int countMcts, bool probabilistic, torch::DeviceType device);
	void connectFourAgainstMiniMaxAi(int depth, cn4::PlayerColor playerColor);
	void runTraining(const TrainingParameters& params);
	void startTwoPlayerConnectFourGame();
	void setTrainingParameters(AlphaZeroTraining<cn4::Board, ConnectFourAdapter>& training, const TrainingParameters& params, int currentIteration);
	void evalConnectFour();

private:
	EvalResult evalConnectFour(std::string netName, int miniMaxDepth, torch::DeviceType device);
	void writeEvaluationResultToFile(int iteration, const EvalResult& result, std::ofstream& file);
	AlphaZeroTrainingParameters getDefaultConnectFourTrainingParameters() const;

	inline static const std::string trainingPath = "NeuralNets/Training/ConnectFour";
	inline static const std::string preTrainedPath = "NeuralNets/PreTrained/ConnectFour";
	int mctsCount = 100;
	int evalMCTSCount = 100;
};


#endif //DEEPREINFORCEMENTLEARNING_CONNECTFOURHANDLER_H

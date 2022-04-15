#ifndef DEEPREINFORCEMENTLEARNING_TICTACTOEHANDLER_H
#define DEEPREINFORCEMENTLEARNING_TICTACTOEHANDLER_H

#include <iostream>
#include <string>
#include <chrono>
#include <fstream>
#include <AlphaZeroTraining.h>
#include <NeuralNetworks/DefaultNeuralNet.h>
#include <TicTacToe/TicTacToeAdapter.h>
#include <TicTacToe/TicTacToe.h>
#include <TicTacToe/MiniMaxAi.h>
#include <MultiThreadingNeuralNetManager.h>
#include "Evaluation.h"
#include "TrainingParameters.h"

class TicTacToeHandler
{
public:
	TicTacToeHandler() = default;
	void ticTacToeAgainstNeuralNetAi(int playerColor, std::string netName, int countMcts, bool probabilistic, torch::DeviceType device);
	void ticTacToeAgainstMiniMaxAi(int playerColor);
	void runTraining(const TrainingParameters& params);
	void setTrainingParameters(AlphaZeroTraining& training, const TrainingParameters& params);
	void startTwoPlayerTicTacToeGame();
	void traininingPerformanceTest(torch::DeviceType device);
	void evalTicTacToe(bool multiThreaded = true);

private:
	EvalResult evalTicTacToeMultiThreaded(std::string netName, torch::DeviceType device);
	EvalResult evalTicTacToe(std::string netName, torch::DeviceType device = torch::kCPU);
	void writeEvaluationResultToFile(int iteration, const EvalResult& result, std::ofstream& file);
	void loadDefaultParametersForAlphaZeroTraining(AlphaZeroTraining& ticTacToeZero);
	void loadPerformanceTestParameters(AlphaZeroTraining& ticTacToeZero);

	inline static const std::string trainingPath = "NeuralNets/Training/TicTacToe";
	inline static const std::string preTrainedPath = "NeuralNets/PreTrained/TicTacToe";
	int mcts_count = 50;
	int evalMCTSCount = 50;
};


#endif //DEEPREINFORCEMENTLEARNING_TICTACTOEHANDLER_H

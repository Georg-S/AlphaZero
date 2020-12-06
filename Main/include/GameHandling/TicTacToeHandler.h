#ifndef DEEPREINFORCEMENTLEARNING_TICTACTOEHANDLER_H
#define DEEPREINFORCEMENTLEARNING_TICTACTOEHANDLER_H

#include <iostream>
#include <string>
#include <chrono>
#include <fstream>
#include <AlphaZeroTraining.h>
#include <NeuralNetworks/DefaultNoResidual.h>
#include <TicTacToe/TicTacToeAdapter.h>
#include <TicTacToe/TicTacToe.h>
#include <TicTacToe/MinMaxAi.h>
#include "Evaluation.h"
#include "TrainingParameters.h"


class TicTacToeHandler {
public:
    TicTacToeHandler();
    ~TicTacToeHandler();

    void ticTacToeAgainstNeuralNetAi(int playerColor, std::string netName, int countMcts, bool probabilistic, torch::DeviceType device);
    void ticTacToeAgainstMinMaxAi(int playerColor);

    void runTraining(const TrainingParameters &params);
    void setTrainingParameters(AlphaZeroTraining &training, const TrainingParameters &params);

    void runTrainingWithAlphaZero(torch::DeviceType device);
    void startTwoPlayerTicTacToeGame();
    void traininingPerformanceTest(torch::DeviceType device);
    void evalTicTacToe();
private:
    EvalResult evalTicTacToe(std::string netName, torch::DeviceType device = torch::kCPU);
	void writeEvaluationResultToFile(const int &iteration, const EvalResult& result, std::ofstream& file);
    void loadDefaultParametersForAlphaZeroTraining(AlphaZeroTraining &ticTacToeZero);
    void loadPerformanceTestParameters(AlphaZeroTraining &ticTacToeZero);

    TicTacToeAdapter adap;
    std::string trainingPath = "NeuralNets/Training/TicTacToe";
    std::string preTrainedPath = "NeuralNets/PreTrained/TicTacToe";
    int mcts_count = 50;
	



	int evalMCTSCount = 50;
};


#endif //DEEPREINFORCEMENTLEARNING_TICTACTOEHANDLER_H

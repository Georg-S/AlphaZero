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
#include "Evaluation.h"
#include "TrainingUtility.h"

class TicTacToeHandler
{
public:
	TicTacToeHandler() = default;
	void ticTacToeAgainstNeuralNetAi(ttt::PlayerColor playerColor, std::string netName, int countMcts, bool probabilistic, torch::DeviceType device);
	void ticTacToeAgainstMiniMaxAi(ttt::PlayerColor playerColor);
	void runTraining(const TrainingParameters& params);
	void setTrainingParameters(AlphaZeroTraining<ttt::Board, TicTacToeAdapter>& training, const TrainingParameters& params, int currentIteration);
	void startTwoPlayerTicTacToeGame();
	void evalTicTacToe();

private:
	EvalResult evalTicTacToe(std::string netName, torch::DeviceType device);
	void writeEvaluationResultToFile(int iteration, const EvalResult& result, std::ofstream& file);
	AlphaZeroTrainingParameters getDefaultTicTacToeTrainingParameters() const;

	inline static const std::string trainingPath = "NeuralNets/Training/TicTacToe";
	inline static const std::string preTrainedPath = "NeuralNets/PreTrained/TicTacToe";
	int mcts_count = 50;
	int evalMCTSCount = 50;
};


#endif //DEEPREINFORCEMENTLEARNING_TICTACTOEHANDLER_H

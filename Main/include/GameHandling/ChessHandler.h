#ifndef DEEPREINFORCEMENTLEARNING_CHESSHANDLER_H
#define DEEPREINFORCEMENTLEARNING_CHESSHANDLER_H

#include <chrono>
#include <AlphaZeroTraining.h>
#include <NeuralNetworks/DefaultNeuralNet.h>
#include <Chess/Chess.h>
#include <Chess/ReducedChessAdapter.h>
#include <Chess/MinMaxAi.h>
#include <AI/NeuralNetAi.h>
#include "Evaluation.h"
#include "TrainingParameters.h"

class ChessHandler {
public:
	ChessHandler();
	~ChessHandler();

	void chessAgainstNeuralNetAi(chess::PieceColor playerColor, std::string netName, int mctsCount, bool randomize, torch::DeviceType device);
	void chessAgainstMinMaxAi(int minMaxDepth, chess::PieceColor playerColor);

	void runTrainingWithDefaultParameters(torch::DeviceType device);
	void runTraining(const TrainingParameters& params);

	static void startTwoPlayerChessGame();
	void traininingPerformanceTest(torch::DeviceType device);
	void setTrainingParameters(AlphaZeroTraining& training, const TrainingParameters& params);

private:
	void loadDefaultParametersForAlphaZeroTraining(AlphaZeroTraining& chessZero);
	void loadPerformanceTestParameters(AlphaZeroTraining& chessZero);

	std::string preTrainedPath = "NeuralNets/PreTrained/Chess";
	std::string trainingPath = "NeuralNets/Training/Chess";


	int minMaxDepth = 3;
	int mctsCount = 800;
};


#endif //DEEPREINFORCEMENTLEARNING_CHESSHANDLER_H

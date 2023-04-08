#ifndef DEEPREINFORCEMENTLEARNING_CHESSHANDLER_H
#define DEEPREINFORCEMENTLEARNING_CHESSHANDLER_H

#include <chrono>
#include <AlphaZeroTraining.h>
#include <Chess/ChessAiAdapter.h>
#include <NeuralNetworks/DefaultNeuralNet.h>
#include <Chess/Chess.h>
#include <Chess/ChessAdapter.h>
#include <Chess/Engine/NegamaxAI.h>
#include <Other/NeuralNetAi.h>
#include "Evaluation.h"
#include "TrainingParameters.h"

class ChessHandler
{
public:
	ChessHandler() = default;
	void chessAgainstNeuralNetAi(ceg::PieceColor playerColor, std::string netName, int mctsCount, bool randomize, torch::DeviceType device);
	void chessAgainstMiniMaxAi(int miniMaxDepth, ceg::PieceColor playerColor);
	void runTraining(const TrainingParameters& params);
	static void startTwoPlayerChessGame();
	void setTrainingParameters(AlphaZeroTraining& training, const TrainingParameters& params);

private:
	AlphaZeroTraining::Parameters getDefaultChessTrainingParameters() const;

	inline static const std::string preTrainedPath = "NeuralNets/PreTrained/Chess";
	inline static const std::string trainingPath = "NeuralNets/Training/Chess";
	int miniMaxDepth = 3;
	int mctsCount = 800;
};


#endif //DEEPREINFORCEMENTLEARNING_CHESSHANDLER_H

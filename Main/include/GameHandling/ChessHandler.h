#ifndef DEEPREINFORCEMENTLEARNING_CHESSHANDLER_H
#define DEEPREINFORCEMENTLEARNING_CHESSHANDLER_H

#include <chrono>
#include <regex>
#include <filesystem>
#include <Chess/ChessAiAdapter.h>
#include <AlphaZeroTrainingTemplate.h>
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
	void setTrainingParameters(AlphaZeroTrainingT<ChessAdapter::GameState, ChessAdapter>& training
		, const TrainingParameters& params, int currentIteration);

private:
	AlphaZeroTrainingParameters getDefaultChessTrainingParameters() const;

	inline static const std::string preTrainedPath = "NeuralNets/PreTrained/Chess";
	inline static const std::string trainingPath = "NeuralNets/Training/Chess";
	int miniMaxDepth = 3;
	int mctsCount = 800;
};


#endif //DEEPREINFORCEMENTLEARNING_CHESSHANDLER_H

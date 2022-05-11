#ifndef DEEPREINFORCEMENTLEARNING_NEURALNETAI_H
#define DEEPREINFORCEMENTLEARNING_NEURALNETAI_H

#include <cfloat>
#include <MonteCarloTreeSearch.h>
#include <NeuralNetworks/NeuralNetwork.h>
#include <Game.h>
#include <AlphaZeroUtility.h>
#include "Utility.h"
#include "Ai.h"


class NeuralNetAi : public Ai
{
public:
	NeuralNetAi(NeuralNetwork* net, Game* game, int actionCount, int mctsCount = 50, bool probabilistic = false, torch::DeviceType device = torch::kCPU);
	int getMove(const std::string& state, int color) override;

private:
	NeuralNetwork* m_net;
	Game* m_game;
	int m_actionCount;
	int m_mctsCount;
	torch::DeviceType m_device;
	bool m_probabilistic = false;
};

#endif //DEEPREINFORCEMENTLEARNING_NEURALNETAI_H

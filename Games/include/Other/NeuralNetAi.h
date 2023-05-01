#ifndef DEEPREINFORCEMENTLEARNING_NEURALNETAI_H
#define DEEPREINFORCEMENTLEARNING_NEURALNETAI_H

#include <cfloat>
#include <NeuralNetworks/NeuralNetwork.h>
#include <MonteCarloTreeSearch.h>
#include <AlphaZeroUtility.h>
#include "Utility.h"
#include "Ai.h"

template <typename GameState, typename Game, bool mockExpansion = false>
class NeuralNetAi : public Ai
{
public:
	NeuralNetAi(NeuralNetwork* net, Game* game, MonteCarloTreeSearchCache<GameState, Game, mockExpansion>* cache, 
		int mctsCount = 50, bool probabilistic = false, torch::DeviceType device = torch::kCPU)
		: m_net(net)
		, m_game(game)
		, m_mctsCount(mctsCount)
		, m_probabilistic(probabilistic)	
		, m_device(device)
	{
		m_mcts = MonteCarloTreeSearch<GameState, Game, mockExpansion>(cache, m_game, device);
	}

	std::string getMove(const std::string& state, int color) override
	{
		m_mcts.search(m_mctsCount, state, m_net, color);
		auto probs = m_mcts.getProbabilities(state);

		if (m_probabilistic)
			return ALZ::getRandomAction(probs);
		else
			return ALZ::getBestAction(probs);
	}

private:
	MonteCarloTreeSearch<GameState, Game> m_mcts;
	NeuralNetwork* m_net;
	Game* m_game;
	int m_mctsCount;
	torch::DeviceType m_device;
	bool m_probabilistic = false;
};

#endif //DEEPREINFORCEMENTLEARNING_NEURALNETAI_H

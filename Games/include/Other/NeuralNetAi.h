#ifndef DEEPREINFORCEMENTLEARNING_NEURALNETAI_H
#define DEEPREINFORCEMENTLEARNING_NEURALNETAI_H

#include <cfloat>
#include <NeuralNetworks/NeuralNetwork.h>
#include <MonteCarloTreeSearchTemplate.h>
#include <AlphaZeroUtility.h>
#include "Utility.h"
#include "Ai.h"

template <typename GameState, typename Game, bool mockExpansion = false>
class NeuralNetAi : public Ai
{
public:
	NeuralNetAi(NeuralNetwork* net, Game* game, MonteCarloTreeSearchCacheT<GameState, Game, mockExpansion>* cache, 
		int mctsCount = 50, bool probabilistic = false, torch::DeviceType device = torch::kCPU)
		: m_net(net)
		, m_game(game)
		, m_actionCount(game->getActionCount())
		, m_mctsCount(mctsCount)
		, m_device(device)
		, m_probabilistic(probabilistic)	
	{
		m_mcts = MonteCarloTreeSearchT<GameState, Game, mockExpansion>(cache, m_game, device);
	}

	int getMove(const GameState& state, int color) override
	{
		m_mcts.search(m_mctsCount, state, m_net, color);
		auto probs = m_mcts.getProbabilities(state);

		if (m_probabilistic)
			return ALZ::getRandomAction(probs);
		else
			return ALZ::getBestAction(probs);
	}

private:
	MonteCarloTreeSearchT<GameState, Game> m_mcts;
	NeuralNetwork* m_net;
	Game* m_game;
	int m_actionCount;
	int m_mctsCount;
	torch::DeviceType m_device;
	bool m_probabilistic = false;
};

#endif //DEEPREINFORCEMENTLEARNING_NEURALNETAI_H

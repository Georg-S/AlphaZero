#ifndef DEEPREINFORCEMENTLEARNING_NEURALNETAI_H
#define DEEPREINFORCEMENTLEARNING_NEURALNETAI_H

#include <cfloat>
#include <memory>
#include <NeuralNetworks/NeuralNetwork.h>
#include <MonteCarloTreeSearch.h>
#include <AlphaZeroUtility.h>
#include "Utility.h"
#include "Ai.h"

template <typename GameState, typename Game, bool mockExpansion = false>
class NeuralNetAi : public Ai
{
public:
	NeuralNetAi(NeuralNetwork* net, Game* game, int mctsCount = 50, bool probabilistic = false, torch::DeviceType device = torch::kCPU)
		: m_net(net)
		, m_game(game)
		, m_mctsCount(mctsCount)
		, m_probabilistic(probabilistic)	
		, m_device(device)
	{
		m_cache = std::make_unique< MonteCarloTreeSearchCache<GameState, Game, mockExpansion>>(device, m_game, m_net);
		m_mcts = std::make_unique<MonteCarloTreeSearch<GameState, Game, mockExpansion>>(m_cache.get(), m_game, device);
	}

	int getMove(const std::string& stateStr, int color) override
	{
		auto state = m_game->getGameStateFromString(stateStr, color);
		
		m_mcts->search(m_mctsCount, state, color);
		auto probs = m_mcts->getProbabilities(state);

		if (m_probabilistic)
			return ALZ::getRandomAction(probs);
		else
			return ALZ::getBestAction(probs);
	}

private:
	std::unique_ptr<MonteCarloTreeSearchCache<GameState, Game, mockExpansion>> m_cache;
	std::unique_ptr<MonteCarloTreeSearch<GameState, Game, mockExpansion>> m_mcts;
	NeuralNetwork* m_net;
	Game* m_game;
	int m_mctsCount;
	torch::DeviceType m_device;
	bool m_probabilistic = false;
};

#endif //DEEPREINFORCEMENTLEARNING_NEURALNETAI_H

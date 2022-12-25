#include "Other/NeuralNetAi.h"

NeuralNetAi::NeuralNetAi(NeuralNetwork* net, Game* game, int actionCount, int mctsCount, bool probabilistic, torch::DeviceType device)
	: m_net(net),
	m_game(game),
	m_actionCount(actionCount),
	m_mctsCount(mctsCount),
	m_device(device),
	m_probabilistic(probabilistic)
{
	m_mcts = MonteCarloTreeSearch(m_actionCount);
}

int NeuralNetAi::getMove(const std::string& state, int color)
{
	m_mcts.search(m_mctsCount, state, m_net, m_game, color, m_device);
	std::vector<float> probs = m_mcts.getProbabilities(state);

	if (m_probabilistic)
		return ALZ::getRandomIndex(probs, 1.0);
	else
		return ALZ::getMaxElementIndex(probs);
}

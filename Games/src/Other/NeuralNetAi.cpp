#include "Other/NeuralNetAi.h"

NeuralNetAi::NeuralNetAi(NeuralNetwork* net, Game* game, int actionCount, int mctsCount, bool probabilistic, torch::DeviceType device)
	: m_net(net),
	m_game(game),
	m_actionCount(actionCount),
	m_mctsCount(mctsCount),
	m_probabilistic(probabilistic),
	m_device(device)
{
}

int NeuralNetAi::getMove(const std::string& state, int color)
{
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(m_actionCount);
	mcts.search(m_mctsCount, state, m_net, m_game, color, m_device);
	std::vector<float> probs = mcts.getProbabilities(state);

	if (m_probabilistic)
		return ALZ::getRandomIndex(probs, 1.0);
	else
		return ALZ::getMaxElementIndex(probs);
}

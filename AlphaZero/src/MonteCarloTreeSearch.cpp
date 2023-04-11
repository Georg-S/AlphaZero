#include "MonteCarloTreeSearch.h"

using namespace ALZ;

MonteCarloTreeSearch::MonteCarloTreeSearch(int actionCount, torch::DeviceType device, float cpuct)
	: m_actionCount(actionCount)
	, m_cpuct(cpuct)
	, m_device(device)
{
}

void MonteCarloTreeSearch::search(int count, std::string strState, NeuralNetwork* net, Game* game, int currentPlayer)
{
	for (int i = 0; i < count; i++)
	{
		search(strState, net, game, currentPlayer);
		m_loopDetection.clear();
	}
}

float MonteCarloTreeSearch::search(const std::string& strState, NeuralNetwork* net, Game* game, int currentPlayer)
{
	m_backProp.clear();
	bool expansionNeeded = false;
	float value = searchWithoutExpansion(strState, game, currentPlayer, &expansionNeeded);
	assert(!m_backProp.empty());

	if (expansionNeeded)
	{
		value = expandNewEncounteredState(m_backProp.back().state, m_backProp.back().player, game, net);
		m_backProp.pop_back();
	}

	backpropagateValue(value);

	return -value;
}

bool MonteCarloTreeSearch::startSearchWithoutExpansion(const std::string& strState, Game* game, int currentPlayer, int count)
{
	m_mctsCount = count;

	return runMultipleSearches(strState, game, currentPlayer);
}

bool MonteCarloTreeSearch::expandAndContinueSearchWithoutExpansion(const std::string& strState, Game* game, int currentPlayer, torch::Tensor valueTens, torch::Tensor probabilities)
{
	deferredExpansion(valueTens, probabilities, game);

	return runMultipleSearches(strState, game, currentPlayer);
}

std::vector<float> MonteCarloTreeSearch::getProbabilities(const std::string& state, float temperature)
{
	assert(m_visited.find(state) != m_visited.end());
	auto statePtr = &(*m_visited.find(state));
	std::vector<float> probs;
	probs.reserve(m_actionCount);
	int countSum = sumValue(m_visitCount[statePtr]);

	for (size_t i = 0; i < m_actionCount; i++) 
	{
		if (m_visitCount[statePtr].find(i) == m_visitCount[statePtr].end())
			probs.emplace_back(0);
		else 
			probs.emplace_back(static_cast<float>(pow(m_visitCount[statePtr][i], 1.f / temperature)) / countSum);
	}

	return probs;
}

torch::Tensor MonteCarloTreeSearch::getExpansionNeuralNetInput(Game* game) const
{
	assert(!m_backProp.empty());
	auto& strState = m_backProp.back().state;
	auto currentPlayer = m_backProp.back().player;

	return game->convertStateToNeuralNetInput(strState, currentPlayer);
}

float MonteCarloTreeSearch::searchWithoutExpansion(const std::string& strState, Game* game, int currentPlayer, bool* expansionNeeded)
{
	if (game->isGameOver(strState))
		return game->gameOverReward(strState, currentPlayer);

	m_backProp.emplace_back(strState, currentPlayer);

	if (m_visited.find(strState) == m_visited.end())
	{
		*expansionNeeded = true;
		return 0;
	}

	auto statePtr = &(*m_visited.find(strState));
	m_loopDetection.emplace(statePtr);
	int bestAction = getActionWithHighestUpperConfidenceBound(statePtr, currentPlayer, game);
	std::string nextStateString = game->makeMove(strState, bestAction, currentPlayer);
	m_backProp.back().bestAction = bestAction;

	auto nextStateItr = m_visited.find(nextStateString);
	if (nextStateItr != m_visited.end()) 
	{
		if (m_loopDetection.find(&(*nextStateItr)) != m_loopDetection.end())
			return 0;
	}

	int nextPlayer = game->getNextPlayer(currentPlayer);

	return searchWithoutExpansion(nextStateString, game, nextPlayer, expansionNeeded);
}

bool MonteCarloTreeSearch::runMultipleSearches(const std::string& strState, Game* game, int currentPlayer)
{
	bool expansionNeeded = false;
	while (m_mctsCount--)
	{
		m_loopDetection.clear();
		float value = searchWithoutExpansion(strState, game, currentPlayer, &expansionNeeded);
		if (expansionNeeded)
			return true;
		else
			backpropagateValue(value);
	}
	return false;
}

void MonteCarloTreeSearch::backpropagateValue(float value)
{
	while (!m_backProp.empty())
	{
		value = -value;
		auto& backProp = m_backProp.back();
		auto& state = backProp.state;
		auto statePtr = &(*m_visited.find(state));
		auto bestAction = backProp.bestAction;
		m_qValues[statePtr][bestAction] = (m_visitCount[statePtr][bestAction] * m_qValues[statePtr][bestAction] + value) / (m_visitCount[statePtr][bestAction] + 1);
		m_visitCount[statePtr][bestAction] += 1;
		m_backProp.pop_back();
	}
}

void MonteCarloTreeSearch::deferredExpansion(torch::Tensor valueTens, torch::Tensor probabilities, Game* game)
{
	assert(!m_backProp.empty());
	auto& expansionStateStr = m_backProp.back().state;
	auto expansionPlayer = m_backProp.back().player;
	auto [iter, flag] = m_visited.emplace(std::move(expansionStateStr));
	auto statePtr = &(*iter);

	for (const auto& move : game->getAllPossibleMoves(*statePtr, expansionPlayer))
		m_probabilities[statePtr].emplace_back(move, *(probabilities[move].data_ptr<float>()));

	float value = *(valueTens[0].data_ptr<float>());
	m_backProp.pop_back();
	backpropagateValue(value);
}

float MonteCarloTreeSearch::expandNewEncounteredState(const std::string& strState, int currentPlayer, Game* game, NeuralNetwork* net)
{
	auto [iter, flag] = m_visited.emplace(strState);
	auto statePtr = &(*iter);
	auto input = game->convertStateToNeuralNetInput(strState, currentPlayer).to(m_device);
	auto [valueTens, rawProbs] = net->calculate(input);

	auto probs = rawProbs[0].detach().to(torch::kCPU);
	for (const auto& move : game->getAllPossibleMoves(strState, currentPlayer)) 
		m_probabilities[statePtr].emplace_back(move, *(probs[move].data_ptr<float>()));

	valueTens = valueTens[0][0].to(torch::kCPU);
	float value = *(valueTens.data_ptr<float>());

	return value;
}

int MonteCarloTreeSearch::getActionWithHighestUpperConfidenceBound(const std::string* statePtr, int currentPlayer, Game* game)
{
	float maxUtility = std::numeric_limits<float>::lowest();
	int bestAction = -1;

	for (const auto&[action, probability] : m_probabilities[statePtr])
	{
		float utility = calculateUpperConfidenceBound(statePtr, action, probability);

		if (utility > maxUtility)
		{
			maxUtility = utility;
			bestAction = action;
		}
	}

	return bestAction;
}

float MonteCarloTreeSearch::calculateUpperConfidenceBound(const std::string* statePtr, int action, float probability)
{
	float buf = sqrt(sumValue(m_visitCount[statePtr])) / (1 + m_visitCount[statePtr][action]);

	return m_qValues[statePtr][action] + m_cpuct * probability * buf;
}

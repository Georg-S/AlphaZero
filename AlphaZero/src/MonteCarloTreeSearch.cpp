#include "MonteCarloTreeSearch.h"

using namespace ALZ;

MonteCarloTreeSearch::MonteCarloTreeSearch(int actionCount, torch::DeviceType device, Game* game, MonteCarloTreeSearchCache* cache, float cpuct)
	: m_actionCount(actionCount)
	, m_cpuct(cpuct)
	, m_device(device)
	, m_game(game)
{
	if (!cache)
		m_cache = new MonteCarloTreeSearchCache(device, m_game);
	else
		m_cache = cache;
}

void MonteCarloTreeSearch::search(int count, const std::string& strState, NeuralNetwork* net, Game* game, int currentPlayer)
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

bool MonteCarloTreeSearch::expandAndContinueSearchWithoutExpansion(const std::string& strState, Game* game, int currentPlayer)
{
	deferredExpansion(game);

	return runMultipleSearches(strState, game, currentPlayer);
}

std::vector<std::pair<int, float>> MonteCarloTreeSearch::getProbabilities(const std::string& state, float temperature)
{
	const auto statePtr = &(*m_cache->encountered.find(state));
	assert(m_visited.find(statePtr) != m_visited.end());

	const int countSum = m_visitCountSum[statePtr];
	std::vector<std::pair<int, float>> probs;
	probs.reserve(m_cache->m_probabilities[statePtr].size());

	for (const auto& [action, visitCount]  : m_visitCount[statePtr]) 
	{
		const float probability = static_cast<float>(pow(visitCount, 1.f / temperature)) / countSum;
		probs.emplace_back(action, probability);
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

void MonteCarloTreeSearch::printMemsize() const
{
	//std::cout << std::endl;
	//std::cout << "Memory Usage:" << std::endl;
	//std::cout << "m_visited: " << memSize(m_visited) << std::endl;
	//std::cout << "m_visitCountSum: " << memSize(m_visitCountSum) << std::endl;
	//std::cout << "m_visitCount: " << memSize(m_visitCount) << std::endl;
	//std::cout << "m_qValues: " << memSize(m_qValues) << std::endl;
	//std::cout << "m_probabilities: " << memSize(m_probabilities) << std::endl;
	//std::cout << "Total: " << memSize(m_visited) + memSize(m_visitCountSum) + memSize(m_visitCount) + memSize(m_qValues) + memSize(m_probabilities) << std::endl;
	//std::cout << std::endl << std::endl;
}

long long MonteCarloTreeSearch::getMemSize() const
{
	return memSize(m_loopDetection) + memSize(m_visited) + memSize(m_visitCountSum) + memSize(m_visitCount) + memSize(m_qValues);
}

float MonteCarloTreeSearch::searchWithoutExpansion(std::string strState, Game* game, int currentPlayer, bool* expansionNeeded)
{
	while (true) // Iterate until we reach a "leaf state" (a state not yet expanded or a game over state)
	{
		if (game->isGameOver(strState))
			return game->gameOverReward(strState, currentPlayer);

		m_backProp.emplace_back(std::move(strState), currentPlayer);
		const auto& currentStateStr(m_backProp.back().state);

		if (m_cache->encountered.find(currentStateStr) == m_cache->encountered.end())
		{
			m_cache->addToExpansion({ currentStateStr, currentPlayer });
			*expansionNeeded = true;
			return 0;
		}

		auto statePtr = &(*m_cache->encountered.find(currentStateStr));

		if (m_visited.find(statePtr) == m_visited.end())
		{
			*expansionNeeded = true;
			return 0;
		}
		m_loopDetection.emplace(statePtr);
		int bestAction = getActionWithHighestUpperConfidenceBound(statePtr, currentPlayer, game);
		m_backProp.back().bestAction = bestAction;
		strState = game->makeMove(currentStateStr, bestAction, currentPlayer);

		auto nextStateItr = m_cache->encountered.find(strState);
		if (nextStateItr != m_cache->encountered.end())
		{
			if (m_loopDetection.find(&(*nextStateItr)) != m_loopDetection.end())
				return 0;
		}
		currentPlayer = game->getNextPlayer(currentPlayer);
	}
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
		auto statePtr = &(*m_cache->encountered.find(state));
		auto bestAction = backProp.bestAction;
		m_qValues[statePtr][bestAction] = (m_visitCount[statePtr][bestAction] * m_qValues[statePtr][bestAction] + value) / (m_visitCount[statePtr][bestAction] + 1);
		m_visitCount[statePtr][bestAction] += 1;
		m_visitCountSum[statePtr] += 1;
		m_backProp.pop_back();
	}
}

void MonteCarloTreeSearch::deferredExpansion(Game* game)
{
	assert(!m_backProp.empty());
	auto& expansionStateStr = m_backProp.back().state;
	auto statePtr = &(*m_cache->encountered.find(expansionStateStr));
	m_visited.emplace(statePtr);

	float value = m_cache->m_values[statePtr];
	m_backProp.pop_back();
	backpropagateValue(value);
}

float MonteCarloTreeSearch::expandNewEncounteredState(const std::string& strState, int currentPlayer, Game* game, NeuralNetwork* net)
{
	m_cache->addToExpansion({ strState , currentPlayer });
	m_cache->convertToNeuralInput();
	m_cache->expand(net);

	auto statePtr = &(*m_cache->encountered.find(strState));
	m_visited.emplace(statePtr);

	return m_cache->m_values[statePtr];
}

int MonteCarloTreeSearch::getActionWithHighestUpperConfidenceBound(const std::string* statePtr, int currentPlayer, Game* game)
{
	float maxUtility = std::numeric_limits<float>::lowest();
	int bestAction = -1;

	for (const auto& [action, probability] : m_cache->m_probabilities[statePtr])
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
	const float buf = sqrt(m_visitCountSum[statePtr]) / (1 + m_visitCount[statePtr][action]);

	return m_qValues[statePtr][action] + m_cpuct * probability * buf;
}

long long MonteCarloTreeSearchCache::printMemsize() const
{
	std::cout << "Encountered states: " << memSize(encountered) << std::endl;
	std::cout << "Probabilities: " << memSize(m_probabilities) << std::endl;
	std::cout << "Values: " << memSize(m_values) << std::endl;

	return memSize(encountered) + memSize(m_probabilities) + memSize(m_values);
}

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
	deferredExpansion(valueTens, probabilities);

	return runMultipleSearches(strState, game, currentPlayer);
}

std::vector<float> MonteCarloTreeSearch::getProbabilities(const std::string& state, float temperature)
{
	assert(m_visited.find(state) != m_visited.end());
	auto strPtr = &(*m_visited.find(state));
	std::vector<float> probs;
	probs.reserve(m_actionCount);
	int countSum = sum(m_visitCount[strPtr]);

	for (int i = 0; i < m_actionCount; i++)
		probs.emplace_back(((float)pow(m_visitCount[strPtr][i], 1.f / temperature)) / countSum);

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

	auto stateStrPtr = &(*m_visited.find(strState));
	m_loopDetection.emplace(stateStrPtr);
	int bestAction = getActionWithHighestUpperConfidenceBound(strState, currentPlayer, game);
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
		auto strPtr = &(*m_visited.find(state));
		auto bestAction = backProp.bestAction;
		m_qValues[state][bestAction] = (m_visitCount[strPtr][bestAction] * m_qValues[state][bestAction] + value) / (m_visitCount[strPtr][bestAction] + 1);
		m_visitCount[strPtr][bestAction] += 1;
		m_backProp.pop_back();
	}
}

void MonteCarloTreeSearch::deferredExpansion(torch::Tensor valueTens, torch::Tensor probabilities)
{
	assert(!m_backProp.empty());
	auto expansionState = m_backProp.back().state;
	m_visited.emplace(expansionState);
	m_probabilities[expansionState] = probabilities;
	fillQValuesAndVisitCount(expansionState);
	float value = *(valueTens[0].data_ptr<float>());
	m_backProp.pop_back();
	backpropagateValue(value);
}

float MonteCarloTreeSearch::expandNewEncounteredState(const std::string& strState, int currentPlayer, Game* game, NeuralNetwork* net)
{
	m_visited.emplace(strState);
	auto input = game->convertStateToNeuralNetInput(strState, currentPlayer).to(m_device);
	auto [valueTens, rawProbs] = net->calculate(input);
	m_probabilities[strState] = rawProbs[0].detach().to(torch::kCPU);
	fillQValuesAndVisitCount(strState);

	valueTens = valueTens[0][0].to(torch::kCPU);
	float value = *(valueTens.data_ptr<float>());

	return value;
}

int MonteCarloTreeSearch::getActionWithHighestUpperConfidenceBound(const std::string& strState, int currentPlayer, Game* game)
{
	float maxUtility = std::numeric_limits<float>::lowest();
	int bestAction = -1;
	auto possibleMoves = game->getAllPossibleMoves(strState, currentPlayer);

	for (auto action : possibleMoves)
	{
		float utility = calculateUpperConfidenceBound(strState, action);

		if (utility > maxUtility)
		{
			maxUtility = utility;
			bestAction = action;
		}
	}

	return bestAction;
}

float MonteCarloTreeSearch::calculateUpperConfidenceBound(const std::string& strState, int action)
{
	auto strPtr = &(*m_visited.find(strState));
	float probability = *(m_probabilities[strState][action].data_ptr<float>());
	float buf = sqrt(sum(m_visitCount[strPtr])) / (1 + m_visitCount[strPtr][action]);

	return m_qValues[strState][action] + m_cpuct * probability * buf;
}

void MonteCarloTreeSearch::fillQValuesAndVisitCount(const std::string& state)
{
	auto strPtr = &(*m_visited.find(state));
	m_qValues[state] = std::vector<float>(m_actionCount, 0.f);
	m_visitCount[strPtr] = std::vector<int>(m_actionCount, 0);
}

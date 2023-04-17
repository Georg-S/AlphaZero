#include "MonteCarloTreeSearch.h"

using namespace ALZ;

MonteCarloTreeSearch::MonteCarloTreeSearch(int actionCount, torch::DeviceType device, float cpuct)
	: m_actionCount(actionCount)
	, m_cpuct(cpuct)
	, m_device(device)
{
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

bool MonteCarloTreeSearch::expandAndContinueSearchWithoutExpansion(const std::string& strState, Game* game, int currentPlayer, torch::Tensor valueTens, torch::Tensor probabilities)
{
	deferredExpansion(valueTens, probabilities, game);

	return runMultipleSearches(strState, game, currentPlayer);
}

std::vector<std::pair<int, float>> MonteCarloTreeSearch::getProbabilities(const std::string& state, float temperature)
{
	assert(m_visited.find(state) != m_visited.end());
	const auto statePtr = &(*m_visited.find(state));
	const int countSum = m_visitCountSum[statePtr];
	std::vector<std::pair<int,float>> probs;
	probs.reserve(m_probabilities[statePtr].size());

	for (size_t i = 0; i < m_actionCount; i++) 
	{
		const float probability = static_cast<float>(pow(m_visitCount[statePtr][i], 1.f / temperature)) / countSum;
		probs.emplace_back(i, probability);
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

float MonteCarloTreeSearch::searchWithoutExpansion(std::string strState, Game* game, int currentPlayer, bool* expansionNeeded)
{
	while (true) // Iterate until we reach a "leaf state" (a state not yet expanded or a game over state)
	{
		if (game->isGameOver(strState))
			return game->gameOverReward(strState, currentPlayer);

		m_backProp.emplace_back(std::move(strState), currentPlayer);
		const auto& currentStateStr(m_backProp.back().state);

		if (m_visited.find(currentStateStr) == m_visited.end())
		{
			*expansionNeeded = true;
			return 0;
		}

		auto statePtr = &(*m_visited.find(currentStateStr));
		m_loopDetection.emplace(statePtr);
		int bestAction = getActionWithHighestUpperConfidenceBound(statePtr, currentPlayer, game);
		strState = game->makeMove(currentStateStr, bestAction, currentPlayer);
		m_backProp.back().bestAction = bestAction;

		auto nextStateItr = m_visited.find(strState);
		if (nextStateItr != m_visited.end())
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
		auto statePtr = &(*m_visited.find(state));
		auto bestAction = backProp.bestAction;
		m_qValues[statePtr][bestAction] = (m_visitCount[statePtr][bestAction] * m_qValues[statePtr][bestAction] + value) / (m_visitCount[statePtr][bestAction] + 1);
		m_visitCount[statePtr][bestAction] += 1;
		m_visitCountSum[statePtr]+=1;
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

	for (const auto& [action, probability] : m_probabilities[statePtr])
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

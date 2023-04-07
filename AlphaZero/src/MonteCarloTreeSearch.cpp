#include "MonteCarloTreeSearch.h"

using namespace ALZ;

MonteCarloTreeSearch::MonteCarloTreeSearch(int actionCount, float cpuct)
	: m_actionCount(actionCount)
	, m_cpuct(cpuct)
{
}

void MonteCarloTreeSearch::search(int count, std::string strState, NeuralNetwork* net, Game* game, int currentPlayer, torch::DeviceType device)
{
	for (int i = 0; i < count; i++)
	{
		specialSearch(strState, net, game, currentPlayer, device);
		//search(strState, net, game, currentPlayer, device);
		m_loopDetection.clear();
	}
}

float MonteCarloTreeSearch::search(std::string strState, NeuralNetwork* net, Game* game, int currentPlayer, torch::DeviceType device)
{
	m_loopDetection[strState] = true;

	if (game->isGameOver(strState))
		return -game->gameOverReward(strState, currentPlayer);

	if (m_visited.find(strState) == m_visited.end())
		return -expandNewEncounteredState(strState, currentPlayer, game, net, device);

	int bestAction = getActionWithHighestUpperConfidenceBound(strState, currentPlayer, game);
	std::string nextStateString = game->makeMove(strState, bestAction, currentPlayer);

	if (m_loopDetection.find(nextStateString) != m_loopDetection.end())
		return 0;

	int nextPlayer = game->getNextPlayer(currentPlayer);
	float value = search(nextStateString, net, game, nextPlayer, device);

	m_qValues[strState][bestAction] = (m_visitCount[strState][bestAction] * m_qValues[strState][bestAction] + value) / (m_visitCount[strState][bestAction] + 1);
	m_visitCount[strState][bestAction] += 1;

	return -value;
}

void MonteCarloTreeSearch::multiThreadedSearch(int count, std::string strState, Game* game,
	int currentPlayer, MultiThreadingNeuralNetManager* threadingManager, torch::DeviceType device)
{
	for (int i = 0; i < count; i++)
	{
		multiThreadedSearch(strState, game, currentPlayer, threadingManager, device);
		m_loopDetection.clear();
	}
}

void MonteCarloTreeSearch::backpropagateValue(float value)
{
	while(!m_backProp.empty())
	{
		value = -value;
		auto& backProp = m_backProp.back();
		auto& state = backProp.state;
		auto bestAction = backProp.bestAction;
		m_qValues[state][bestAction] = (m_visitCount[state][bestAction] * m_qValues[state][bestAction] + value) / (m_visitCount[state][bestAction] + 1);
		m_visitCount[state][bestAction] += 1;
		m_backProp.pop_back();
	}
}

void MonteCarloTreeSearch::deferredExpansion(torch::Tensor valueTens, torch::Tensor probabilities)
{
	assert(!m_backProp.empty());
	auto expansionState = m_backProp.back().state;
	m_visited[expansionState] = true;
	m_probabilities[expansionState] = probabilities;
	fillQValuesAndVisitCount(expansionState);
	float value = *(valueTens[0].data_ptr<float>());
	m_backProp.pop_back();
	backpropagateValue(value);
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

bool MonteCarloTreeSearch::specialSearch(const std::string& strState, Game* game, int currentPlayer, int count)
{
	m_mctsCount = count;

	return runMultipleSearches(strState, game, currentPlayer);
}

bool MonteCarloTreeSearch::continueSpecialSearch(const std::string& strState, Game* game, int currentPlayer, torch::Tensor valueTens, torch::Tensor probabilities)
{
	deferredExpansion(valueTens, probabilities);

	return runMultipleSearches(strState, game, currentPlayer);
}

void MonteCarloTreeSearch::specialSearch(const std::string& strState, NeuralNetwork* net, Game* game, int currentPlayer, torch::DeviceType device)
{
	m_backProp.clear();
	bool expansionNeeded = false;
	float value = searchWithoutExpansion(strState, game, currentPlayer, &expansionNeeded);
	assert(!m_backProp.empty());

	if (expansionNeeded)
	{
		value = expandNewEncounteredState(m_backProp.back().state, m_backProp.back().player, game, net, device);
		m_backProp.pop_back();
	}

	backpropagateValue(value);
}

float MonteCarloTreeSearch::searchWithoutExpansion(const std::string& strState, Game* game, int currentPlayer, bool* expansionNeeded)
{
	m_loopDetection[strState] = true;

	if (game->isGameOver(strState))
		return game->gameOverReward(strState, currentPlayer);

	m_backProp.emplace_back(strState, currentPlayer);

	if (m_visited.find(strState) == m_visited.end())
	{
		*expansionNeeded = true;
		return 0;
	}

	int bestAction = getActionWithHighestUpperConfidenceBound(strState, currentPlayer, game);
	std::string nextStateString = game->makeMove(strState, bestAction, currentPlayer);
	m_backProp.back().bestAction = bestAction;

	if (m_loopDetection.find(nextStateString) != m_loopDetection.end())
		return 0;

	int nextPlayer = game->getNextPlayer(currentPlayer);

	return searchWithoutExpansion(nextStateString, game, nextPlayer, expansionNeeded);
}

float MonteCarloTreeSearch::multiThreadedSearch(std::string strState, Game* game,
	int currentPlayer, MultiThreadingNeuralNetManager* threadingManager, torch::DeviceType device)
{
	m_loopDetection[strState] = true;

	if (game->isGameOver(strState))
		return -game->gameOverReward(strState, currentPlayer);

	if (m_visited.find(strState) == m_visited.end())
		return -multiThreadedExpandNewState(strState, currentPlayer, game, threadingManager, device);

	int bestAction = getActionWithHighestUpperConfidenceBound(strState, currentPlayer, game);
	std::string nextStateString = game->makeMove(strState, bestAction, currentPlayer);

	if (m_loopDetection.find(nextStateString) != m_loopDetection.end())
		return 0;

	int nextPlayer = game->getNextPlayer(currentPlayer);
	float value = multiThreadedSearch(nextStateString, game, nextPlayer, threadingManager, device);

	m_qValues[strState][bestAction] = (m_visitCount[strState][bestAction] * m_qValues[strState][bestAction] + value) / (m_visitCount[strState][bestAction] + 1);
	m_visitCount[strState][bestAction] += 1;

	return -value;
}

void MonteCarloTreeSearch::fillQValuesAndVisitCount(const std::string& state)
{
	m_qValues[state] = std::vector<float>(m_actionCount, 0.f);
	m_visitCount[state] = std::vector<int>(m_actionCount, 0);
}

torch::Tensor MonteCarloTreeSearch::getExpansionNeuralNetInput(Game* game, torch::DeviceType device) const
{
	assert(!m_backProp.empty());
	auto& strState = m_backProp.back().state;
	auto currentPlayer = m_backProp.back().player;

	return game->convertStateToNeuralNetInput(strState, currentPlayer, device);
}

void MonteCarloTreeSearch::clearAll()
{
	m_visitCount.clear();
	m_visited.clear();
	m_qValues.clear();
	m_probabilities.clear();
	m_loopDetection.clear();
}

std::vector<float> MonteCarloTreeSearch::getProbabilities(const std::string& state, float temperature)
{
	std::vector<float> probs;
	probs.reserve(m_actionCount);
	int countSum = sum(m_visitCount[state]);

	for (int i = 0; i < m_actionCount; i++)
		probs.emplace_back(((float)pow(m_visitCount[state][i], 1.f / temperature)) / countSum);

	return probs;
}

float MonteCarloTreeSearch::expandNewEncounteredState(const std::string& strState, int currentPlayer, Game* game, NeuralNetwork* net, torch::DeviceType device)
{
	m_visited[strState] = true;
	auto input = game->convertStateToNeuralNetInput(strState, currentPlayer, device);
	auto [valueTens, rawProbs] = net->calculate(input);
	m_probabilities[strState] = rawProbs[0].detach().to(torch::kCPU);
	fillQValuesAndVisitCount(strState);

	valueTens = valueTens[0][0].to(torch::kCPU);
	float value = *(valueTens.data_ptr<float>());

	return value;
}

float MonteCarloTreeSearch::multiThreadedExpandNewState(const std::string& strState, int currentPlayer, Game* game, MultiThreadingNeuralNetManager* threadingManager, torch::DeviceType device)
{
	m_visited[strState] = true;
	auto input = game->convertStateToNeuralNetInput(strState, currentPlayer, device);
	const int resultIndex = threadingManager->addInputThreadSafe(input);

	threadingManager->handleWaitingAndWakeup();

	auto netPredict = threadingManager->getOutput(resultIndex);
	torch::Tensor rawProbs = std::get<1>(netPredict).detach().to(torch::kCPU);
	m_probabilities[strState] = rawProbs;
	fillQValuesAndVisitCount(strState);

	torch::Tensor valueTens = std::get<0>(netPredict)[0].to(torch::kCPU);
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
	float probability = *(m_probabilities[strState][action].data_ptr<float>());
	float buf = sqrt(sum(m_visitCount[strState])) / (1 + m_visitCount[strState][action]);

	return m_qValues[strState][action] + m_cpuct * probability * buf;
}

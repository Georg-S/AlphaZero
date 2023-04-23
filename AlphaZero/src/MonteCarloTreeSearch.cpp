#include "MonteCarloTreeSearch.h"

using namespace ALZ;

// Used for debugging memory usage of MCTS
static constexpr bool MOCK_EXPAND = false;

MonteCarloTreeSearchCache::MonteCarloTreeSearchCache(torch::DeviceType device, Game* game)
	: m_device(device)
	, m_game(game) 
{
}

void MonteCarloTreeSearchCache::addToExpansion(ExpansionData&& data)
{
	toExpand.emplace(std::move(data));
}

void MonteCarloTreeSearchCache::convertToNeuralInput()
{
	torch::NoGradGuard no_grad;
	//if (MOCK_EXPAND)
	//	return;
	for (const auto& state : toExpand)
		addToInput(state);
}

void MonteCarloTreeSearchCache::expand(NeuralNetwork* net)
{
	torch::NoGradGuard no_grad;
	m_values.clear();
	m_probabilities.clear();

	if (MOCK_EXPAND) 
	{
		size_t counter = 0;
		for (const auto& state : toExpand)
		{
			m_values[state.state] = ALZ::getRandomNumber(-1.0, 1.0);

			for (const auto& move : m_game->getAllPossibleMoves(state.state, state.currentPlayer))
				m_probabilities[state.state].emplace_back(move, ALZ::getRandomNumber(0.0, 1.0));
		}
	}
	else 
	{
		calculateOutput(net);

		size_t counter = 0;
		for (const auto& state : toExpand)
		{
			auto [val, probs] = getOutput(counter++);

			m_values[state.state] = *(val[0].data_ptr<float>());

			for (const auto& move : m_game->getAllPossibleMoves(state.state, state.currentPlayer))
				m_probabilities[state.state].emplace_back(move, *(probs[move].data_ptr<float>()));
		}
	}

	toExpand.clear();
}

long long MonteCarloTreeSearchCache::getMemSize() const
{
	return memSize(m_probabilities) + memSize(m_values);
}

void MonteCarloTreeSearchCache::clear()
{
	m_probabilities.clear();
	m_input = torch::Tensor{};
	m_outputValues = torch::Tensor{};
}

int MonteCarloTreeSearchCache::addToInput(ExpansionData data)
{
	if (m_maxSize == 0) 
	{
		m_input = m_game->convertStateToNeuralNetInput(data.state, data.currentPlayer);
		m_maxSize++;
	}
	else if (m_currentInputSize >= m_maxSize)
	{
		auto tens = m_game->convertStateToNeuralNetInput(data.state, data.currentPlayer);
		m_input = torch::cat({ m_input, tens }, 0);
		m_maxSize++;
	}
	else 
	{
		m_game->convertStateToNeuralNetInput(data.state, data.currentPlayer, m_input[m_currentInputSize]);
	}

	return m_currentInputSize++;
}

void MonteCarloTreeSearchCache::calculateOutput(NeuralNetwork* net)
{
	if (m_currentInputSize == 0)
		return;

	auto device_input = m_input.to(m_device);
	auto rawOutput = net->calculate(device_input);
	m_outputSize = m_currentInputSize;
	m_currentInputSize = 0;

	m_outputValues = std::get<0>(rawOutput).detach().to(torch::kCPU);
	m_outputProbabilities = std::get<1>(rawOutput).detach().to(torch::kCPU);
}

std::pair<torch::Tensor, torch::Tensor> MonteCarloTreeSearchCache::getOutput(size_t index)
{
	assert(index < m_outputSize);

	return { m_outputValues[index], m_outputProbabilities[index] };
}

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

void MonteCarloTreeSearch::search(int count, const std::string& strState, NeuralNetwork* net, int currentPlayer)
{
	for (int i = 0; i < count; i++)
	{
		search(strState, net, currentPlayer);
		m_loopDetection.clear();
	}
}

float MonteCarloTreeSearch::search(const std::string& strState, NeuralNetwork* net, int currentPlayer)
{
	m_backProp.clear();
	bool expansionNeeded = false;
	float value = searchWithoutExpansion(strState, currentPlayer, &expansionNeeded);
	assert(!m_backProp.empty());

	if (expansionNeeded)
	{
		value = expandNewEncounteredState(m_backProp.back().state, m_backProp.back().player, net);
		m_backProp.pop_back();
	}

	backpropagateValue(value);

	return -value;
}

bool MonteCarloTreeSearch::startSearchWithoutExpansion(const std::string& strState, int currentPlayer, int count)
{
	m_mctsCount = count;

	return runMultipleSearches(strState, currentPlayer);
}

bool MonteCarloTreeSearch::expandAndContinueSearchWithoutExpansion(const std::string& strState, int currentPlayer)
{
	deferredExpansion();

	return runMultipleSearches(strState, currentPlayer);
}

std::vector<std::pair<int, float>> MonteCarloTreeSearch::getProbabilities(const std::string& state, float temperature)
{
	const auto statePtr = &(*m_visited.find(state));
	assert(statePtr);

	const int countSum = getVisitCountSum(statePtr);
	std::vector<std::pair<int, float>> probs;
	probs.reserve(m_probabilities[statePtr].size());

	for (const auto& [action, visitCount] : m_visitCount[statePtr])
	{
		const float probability = static_cast<float>(pow(visitCount, 1.f / temperature)) / countSum;
		probs.emplace_back(action, probability);
	}

	return probs;
}

long long MonteCarloTreeSearch::getMemSize() const
{
	return memSize(m_loopDetection) + memSize(m_visited) + memSize(m_visitCount) + memSize(m_qValues);
}

float MonteCarloTreeSearch::searchWithoutExpansion(std::string strState, int currentPlayer, bool* expansionNeeded)
{
	while (true) // Iterate until we reach a "leaf state" (a state not yet expanded or a game over state)
	{
		if (m_game->isGameOver(strState))
			return m_game->gameOverReward(strState, currentPlayer);

		m_backProp.emplace_back(std::move(strState), currentPlayer);
		const auto& currentStateStr(m_backProp.back().state);

		auto currentStateItr = m_visited.find(currentStateStr);
		if (currentStateItr == m_visited.end())
		{
			m_cache->addToExpansion({ currentStateStr, currentPlayer });
			*expansionNeeded = true;
			return 0;
		}

		auto statePtr = &(*currentStateItr);
		m_loopDetection.emplace(statePtr);
		int bestAction = getActionWithHighestUpperConfidenceBound(statePtr, currentPlayer);
		m_backProp.back().bestAction = bestAction;
		strState = m_game->makeMove(currentStateStr, bestAction, currentPlayer);

		auto nextStateItr = m_visited.find(strState);
		if (nextStateItr != m_visited.end())
		{
			if (m_loopDetection.find(&(*nextStateItr)) != m_loopDetection.end())
				return 0;
		}
		currentPlayer = m_game->getNextPlayer(currentPlayer);
	}
}

bool MonteCarloTreeSearch::runMultipleSearches(const std::string& strState, int currentPlayer)
{
	bool expansionNeeded = false;
	while (m_mctsCount--)
	{
		m_loopDetection.clear();
		float value = searchWithoutExpansion(strState, currentPlayer, &expansionNeeded);
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

void MonteCarloTreeSearch::deferredExpansion()
{
	assert(!m_backProp.empty());
	const auto [iter, success] = m_visited.emplace(std::move(m_backProp.back().state));
	const auto statePtr = &(*iter);
	float value = m_cache->m_values[*statePtr];
	m_probabilities[statePtr] = m_cache->m_probabilities[*statePtr];
	if (m_probabilities[statePtr].empty())
		std::cout << "Problem" << std::endl;

	m_backProp.pop_back();
	backpropagateValue(value);
}

float MonteCarloTreeSearch::expandNewEncounteredState(const std::string& strState, int currentPlayer, NeuralNetwork* net)
{
	m_cache->addToExpansion({ strState , currentPlayer });
	m_cache->convertToNeuralInput();
	m_cache->expand(net);
	const auto [iter, success] = m_visited.emplace(strState);
	const auto statePtr = &(*iter);
	m_probabilities[statePtr] = m_cache->m_probabilities[*statePtr];

	return m_cache->m_values[strState];
}

int MonteCarloTreeSearch::getActionWithHighestUpperConfidenceBound(const std::string* statePtr, int currentPlayer)
{
	float maxUtility = std::numeric_limits<float>::lowest();
	int bestAction = -1;

	const auto stateVisitCountSum = getVisitCountSum(statePtr);
	for (const auto& [action, probability] : m_probabilities[statePtr])
	{
		float utility = calculateUpperConfidenceBound(statePtr, action, probability, stateVisitCountSum);

		if (utility > maxUtility)
		{
			maxUtility = utility;
			bestAction = action;
		}
	}

	return bestAction;
}

float MonteCarloTreeSearch::calculateUpperConfidenceBound(const std::string* statePtr, int action, float probability, unsigned int stateVisitCountSum)
{
	const float buf = sqrt(stateVisitCountSum) / (1 + m_visitCount[statePtr][action]);

	return m_qValues[statePtr][action] + m_cpuct * probability * buf;
}

unsigned int MonteCarloTreeSearch::getVisitCountSum(const std::string* statePtr) const
{
	assert(statePtr);
	unsigned int countSum = 0;
	const auto iter = m_visitCount.find(statePtr);
	if (iter == m_visitCount.end())
		return 0;
	for (const auto& [action, visitCount] : iter->second)
		countSum += visitCount;

	return countSum;
}

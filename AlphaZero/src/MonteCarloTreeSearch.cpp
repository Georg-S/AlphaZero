#include "MonteCarloTreeSearch.h"

MonteCarloTreeSearch::MonteCarloTreeSearch()
{

}

MonteCarloTreeSearch::MonteCarloTreeSearch(int actionCount, float cpuct)
{
	this->actionCount = actionCount;
	this->cpuct = cpuct;
}

void MonteCarloTreeSearch::search(int countBatches, int countPerBatch, std::string startState,
	NeuralNetwork* net, Game* game, int currentPlayer, torch::DeviceType device)
{
	bool allStatesExpanded = false;

	for (int i = 0; (i < countBatches) && (!allStatesExpanded); i++)
	{
		int initialCount = 0;
		loopDetection.clear();
		toUpdateValues.clear();
		undiscoveredStates = torch::Tensor();

		searchBatch(countPerBatch, initialCount, startState, game, currentPlayer, std::vector<StateActionValue>(), device);
		calculateNetOutput(net);
		updateTree();

		if (undiscoveredStates.numel() == 0)
			allStatesExpanded = true;
	}
	if (allStatesExpanded)
		search(countBatches * countPerBatch, startState, net, game, currentPlayer, device);
}

void MonteCarloTreeSearch::searchBatch(int countPerBatch, int& currentCount, std::string strState,
	Game* game, int currentPlayer, std::vector<StateActionValue> currentPath, torch::DeviceType device)
{
	if (currentCount >= countPerBatch)
		return;

	if (loopDetection.find(strState) != loopDetection.end())
	{
		currentPath.push_back(StateActionValue{ strState, -1, 0.f });
		toUpdateValues.push_back(currentPath);
		currentCount++;
		return;
	}

	if (game->isGameOver(strState))
	{
		float value = game->gameOverReward(strState, currentPlayer);
		currentPath.push_back(StateActionValue{ strState, -1, value });
		toUpdateValues.push_back(currentPath);
		currentCount++;
		return;
	}

	if (visited.find(strState) == visited.end())
	{
		if (alreadyInExpansion.find(strState) != alreadyInExpansion.end())
			return;

		currentPath.push_back(StateActionValue{ strState, -1, INT_MIN });
		toUpdateValues.push_back(currentPath);

		auto toExpand = game->convertStateToNeuralNetInput(strState, currentPlayer, device);
		if (undiscoveredStates.numel() == 0)
			undiscoveredStates = toExpand;
		else
			undiscoveredStates = torch::cat({ undiscoveredStates, toExpand }, 0);

		alreadyInExpansion.insert(strState);
		currentCount++;
		return;
	}

	std::vector<int> possibleMoves = game->getAllPossibleMoves(strState, currentPlayer);
	std::vector<std::tuple<float, int>> actionValuePair;
	for (int action : possibleMoves)
		actionValuePair.push_back(std::make_tuple(calculateUpperConfidenceBound(strState, action), action));

	std::sort(actionValuePair.begin(), actionValuePair.end(), std::greater<>());

	for (int i = 0; i < actionValuePair.size(); i++)
	{
		std::vector<StateActionValue> currentPathCopy = currentPath;
		int action = std::get<1>(actionValuePair[i]);
		int nextPlayer = game->getNextPlayer(currentPlayer);
		std::string nextState = game->makeMove(strState, action, currentPlayer);
		currentPathCopy.push_back(StateActionValue{ strState, action, INT_MIN });

		searchBatch(countPerBatch, currentCount, nextState, game, nextPlayer, currentPathCopy, device);
	}
}

void MonteCarloTreeSearch::calculateNetOutput(NeuralNetwork* net)
{
	if (undiscoveredStates.numel() != 0)
		resultOfExpansion = net->calculate(undiscoveredStates);
}

void MonteCarloTreeSearch::updateTree()
{
	int tensorIndex = 0;
	for (auto treePath : toUpdateValues)
	{
		StateActionValue treeEnd = treePath[treePath.size() - 1];

		if (treeEnd.value == INT_MIN && treeEnd.action == -1)
		{
			torch::Tensor rawProbs = std::get<1>(resultOfExpansion)[tensorIndex].detach().to(torch::kCPU);
			probabilities[treeEnd.state] = rawProbs;
			fillQValuesAndVisitCount(treeEnd.state);

			torch::Tensor valueTens = std::get<0>(resultOfExpansion)[tensorIndex][0].to(torch::kCPU);
			treeEnd.value = *(valueTens.data_ptr<float>());
			tensorIndex++;
			visited[treeEnd.state] = true;
		}

		float value = treeEnd.value;

		for (int i = treePath.size() - 2; i >= 0; i--)
		{
			value = -value;
			const std::string& state = treePath[i].state;
			const int action = treePath[i].action;

			qValues[state][action] = (visitCount[state][action] * qValues[state][action] + value) / (visitCount[state][action] + 1);
			visitCount[state][action] += 1;
		}
	}
}

void MonteCarloTreeSearch::search(int count, std::string strState, NeuralNetwork* net, Game* game, int currentPlayer, torch::DeviceType device)
{
	for (int i = 0; i < count; i++) {
		search(strState, net, game, currentPlayer, device);
		loopDetection.clear();
	}
}

float MonteCarloTreeSearch::search(std::string strState, NeuralNetwork* net, Game* game, int currentPlayer, torch::DeviceType device)
{
	loopDetection[strState] = true;

	if (game->isGameOver(strState))
		return -game->gameOverReward(strState, currentPlayer);

	if (visited.find(strState) == visited.end())
		return -expandNewEncounteredState(strState, currentPlayer, game, net, device);

	int bestAction = getActionWithHighestUpperConfidenceBound(strState, currentPlayer, game);
	std::string nextStateString = game->makeMove(strState, bestAction, currentPlayer);

	if (!(loopDetection.find(nextStateString) == loopDetection.end()))
		return 0;

	int nextPlayer = game->getNextPlayer(currentPlayer);
	float value = search(nextStateString, net, game, nextPlayer, device);

	qValues[strState][bestAction] = (visitCount[strState][bestAction] * qValues[strState][bestAction] + value) / (visitCount[strState][bestAction] + 1);
	visitCount[strState][bestAction] += 1;

	return -value;
}

void MonteCarloTreeSearch::fillQValuesAndVisitCount(const std::string& state)
{
	qValues[state] = std::vector<float>(actionCount, 0.f);
	visitCount[state] = std::vector<int>(actionCount, 0.f);
}

void MonteCarloTreeSearch::clearAll()
{
	visitCount.clear();
	visited.clear();
	qValues.clear();
	probabilities.clear();

	toUpdateValues.clear();
	undiscoveredStates = torch::Tensor();
	alreadyInExpansion.clear();
	loopDetection.clear();
}

std::vector<float> MonteCarloTreeSearch::getProbabilities(const std::string& state, float temperature)
{
	std::vector<float> probs;
	int countSum = sum(visitCount[state]);

	for (int i = 0; i < actionCount; i++)
		probs.push_back(((float)pow(visitCount[state][i], 1.f / temperature)) / countSum);

	return probs;
}

int MonteCarloTreeSearch::sum(const std::vector<int>& vector)
{
	int sum_of_elems = 0;
	for (int n : vector)
		sum_of_elems += n;

	return sum_of_elems;
}

float MonteCarloTreeSearch::expandNewEncounteredState(const std::string& strState, int currentPlayer, Game* game, NeuralNetwork* net, torch::DeviceType device)
{
	visited[strState] = true;
	auto input = game->convertStateToNeuralNetInput(strState, currentPlayer, device);
	auto netPredict = net->calculate(input);
	torch::Tensor rawProbs = std::get<1>(netPredict)[0].detach().to(torch::kCPU);
	probabilities[strState] = rawProbs;
	fillQValuesAndVisitCount(strState);

	torch::Tensor valueTens = std::get<0>(netPredict)[0][0].to(torch::kCPU);
	float value = *(valueTens.data_ptr<float>());

	return value;
}

int MonteCarloTreeSearch::getActionWithHighestUpperConfidenceBound(const std::string& strState, int currentPlayer, Game* game)
{
	float maxUtility = -std::numeric_limits<float>::max();
	int bestAction = -1;
	std::vector<int> possibleMoves = game->getAllPossibleMoves(strState, currentPlayer);

	for (int i = 0; i < possibleMoves.size(); i++) {
		int action = possibleMoves[i];
		float utility = calculateUpperConfidenceBound(strState, action);

		if (utility > maxUtility) {
			maxUtility = utility;
			bestAction = action;
		}
	}

	return bestAction;
}

float MonteCarloTreeSearch::calculateUpperConfidenceBound(const std::string& strState, int action)
{
	float probability = *(probabilities[strState][action].data_ptr<float>());
	float buf = sqrt(sum(visitCount[strState])) / (1 + visitCount[strState][action]);

	return qValues[strState][action] + cpuct * probability * buf;
}


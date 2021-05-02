#include "MonteCarloTreeSearch.h"

MonteCarloTreeSearch::MonteCarloTreeSearch()
{

}

MonteCarloTreeSearch::MonteCarloTreeSearch(int actionCount, float cpuct)
{
	this->actionCount = actionCount;
	this->cpuct = cpuct;
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

void MonteCarloTreeSearch::multiThreadedSearch(int count, std::string strState, Game* game,
	int currentPlayer, MultiThreadingNeuralNetManager* threadingManager, torch::DeviceType device)
{
	for (int i = 0; i < count; i++)
	{
		multiThreadedSearch(strState, game, currentPlayer, threadingManager, device);
		loopDetection.clear();
	}
}

float MonteCarloTreeSearch::multiThreadedSearch(std::string strState, Game* game,
	int currentPlayer, MultiThreadingNeuralNetManager* threadingManager, torch::DeviceType device)
{
	loopDetection[strState] = true;

	if (game->isGameOver(strState))
		return -game->gameOverReward(strState, currentPlayer);

	if (visited.find(strState) == visited.end())
		return -multiThreadedExpandNewState(strState, currentPlayer, game, threadingManager, device);

	int bestAction = getActionWithHighestUpperConfidenceBound(strState, currentPlayer, game);
	std::string nextStateString = game->makeMove(strState, bestAction, currentPlayer);

	if (!(loopDetection.find(nextStateString) == loopDetection.end()))
		return 0;

	int nextPlayer = game->getNextPlayer(currentPlayer);
	float value = multiThreadedSearch(nextStateString, game, nextPlayer, threadingManager, device);

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

float MonteCarloTreeSearch::multiThreadedExpandNewState(const std::string& strState, int currentPlayer, Game* game, MultiThreadingNeuralNetManager* threadingManager, torch::DeviceType device)
{
	visited[strState] = true;
	auto input = game->convertStateToNeuralNetInput(strState, currentPlayer, device);
	const int resultIndex = threadingManager->addInputThreadSafe(input);

	threadingManager->handleWaitingAndWakeup();

	auto netPredict = threadingManager->getOutput(resultIndex);
	torch::Tensor rawProbs = std::get<1>(netPredict).detach().to(torch::kCPU);
	probabilities[strState] = rawProbs;
	fillQValuesAndVisitCount(strState);

	torch::Tensor valueTens = std::get<0>(netPredict)[0].to(torch::kCPU);
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

#ifndef DEEPREINFORCEMENTLEARNING_MONTECARLOTREESEARCH_H
#define DEEPREINFORCEMENTLEARNING_MONTECARLOTREESEARCH_H

#include <map>
#include <vector>
#include <string>
#include <torch/torch.h>
#include <limits>
#include <utility>
#include <mutex>
#include "MultiThreadingNeuralNetManager.h"
#include "Game.h"
#include "NeuralNetworks/NeuralNetwork.h"

struct StateActionValue
{
	std::string state;
	int action;
	float value;
};

class MonteCarloTreeSearch
{
public:
	MonteCarloTreeSearch();
	MonteCarloTreeSearch(int actionCount, float cpuct = 1.0);
	void search(int count, std::string strState, NeuralNetwork* net, Game* game,
		int currentPlayer, torch::DeviceType device = torch::kCPU);
	float search(std::string strState, NeuralNetwork* net, Game* game, int currentPlayer, torch::DeviceType device = torch::kCPU);
	void multiThreadedSearch(int count, std::string strState, NeuralNetwork* net, Game* game, int currentPlayer,
		MultiThreadingNeuralNetManager* threadingManager, torch::DeviceType device = torch::kCPU);
	float multiThreadedSearch(std::string strState, NeuralNetwork* net, Game* game, int currentPlayer,
		MultiThreadingNeuralNetManager* threadingManager,
		torch::DeviceType device = torch::kCPU);
	std::vector<float> getProbabilities(const std::string& state, float temperature = 1.0);
	static int sum(const std::vector<int>& vector);
	void clearAll();
	void fillQValuesAndVisitCount(const std::string& state);

private:
	float expandNewEncounteredState(const std::string& strState, int currentPlayer, Game* game, NeuralNetwork* net, torch::DeviceType device);
	float multiThreadedExpandNewState(const std::string& strState, int currentPlayer, Game* game, NeuralNetwork* net, 
		MultiThreadingNeuralNetManager* threadingManager, torch::DeviceType device);
	int getActionWithHighestUpperConfidenceBound(const std::string& strState, int currentPlayer, Game* game);
	float calculateUpperConfidenceBound(const std::string& strState, int action);

	int actionCount;
	float cpuct;
	std::map<std::string, bool> loopDetection;
	std::map<std::string, bool> visited;
	std::map<std::string, std::vector<int>> visitCount;
	std::map<std::string, std::vector<float>> qValues;
	std::map<std::string, torch::Tensor> probabilities;

	std::vector<std::vector<StateActionValue>> toUpdateValues;
	torch::Tensor undiscoveredStates;
	std::tuple<torch::Tensor, torch::Tensor> resultOfExpansion;
	std::set<std::string> alreadyInExpansion;
};

#endif //DEEPREINFORCEMENTLEARNING_MONTECARLOTREESEARCH_H

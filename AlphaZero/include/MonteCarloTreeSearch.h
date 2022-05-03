#ifndef DEEPREINFORCEMENTLEARNING_MONTECARLOTREESEARCH_H
#define DEEPREINFORCEMENTLEARNING_MONTECARLOTREESEARCH_H

#include <map>
#include <vector>
#include <string>
#include <limits>
#include <utility>
#include <mutex>
// Libtorch has many warnings which clutter the output, so we ignore them
#pragma warning(push, 0)
#include <torch/torch.h>
#pragma warning(pop)
#include "Utility.h"
#include "MultiThreadingNeuralNetManager.h"
#include "Game.h"
#include "NeuralNetworks/NeuralNetwork.h"

class MonteCarloTreeSearch
{
public:
	MonteCarloTreeSearch() = default;
	MonteCarloTreeSearch(int actionCount, float cpuct = 1.0);
	void search(int count, std::string strState, NeuralNetwork* net, Game* game,
		int currentPlayer, torch::DeviceType device = torch::kCPU);
	float search(std::string strState, NeuralNetwork* net, Game* game, int currentPlayer, torch::DeviceType device = torch::kCPU);
	void multiThreadedSearch(int count, std::string strState, Game* game, int currentPlayer,
		MultiThreadingNeuralNetManager* threadingManager, torch::DeviceType device = torch::kCPU);
	float multiThreadedSearch(std::string strState, Game* game, int currentPlayer,
		MultiThreadingNeuralNetManager* threadingManager,
		torch::DeviceType device = torch::kCPU);
	std::vector<float> getProbabilities(const std::string& state, float temperature = 1.0);
	void clearAll();
	void fillQValuesAndVisitCount(const std::string& state);

private:
	float expandNewEncounteredState(const std::string& strState, int currentPlayer, Game* game, NeuralNetwork* net, torch::DeviceType device);
	float multiThreadedExpandNewState(const std::string& strState, int currentPlayer, Game* game,
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
};

#endif //DEEPREINFORCEMENTLEARNING_MONTECARLOTREESEARCH_H

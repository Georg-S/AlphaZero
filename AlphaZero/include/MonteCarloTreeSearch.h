#ifndef DEEPREINFORCEMENTLEARNING_MONTECARLOTREESEARCH_H
#define DEEPREINFORCEMENTLEARNING_MONTECARLOTREESEARCH_H

#include <map>
#include <vector>
#include <string>
#include <limits>
#include <utility>
#include <mutex>
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
// Libtorch has many warnings which clutter the output, so we ignore them
#pragma warning(push, 0)
#include <torch/torch.h>
#pragma warning(pop)
#include "AlphaZeroUtility.h"
#include "MultiThreadingNeuralNetManager.h"
#include "Game.h"
#include "NeuralNetworks/NeuralNetwork.h"

class MonteCarloTreeSearchCache
{
public:
	friend class MonteCarloTreeSearch;

	struct ExpansionData
	{
		std::string state;
		int currentPlayer;

		friend bool operator<(const ExpansionData& lhs, const ExpansionData& rhs)
		{
			return lhs.state < rhs.state;
		}
	};

	MonteCarloTreeSearchCache(torch::DeviceType device, Game* game);
	MonteCarloTreeSearchCache(const MonteCarloTreeSearchCache&) = delete;
	MonteCarloTreeSearchCache& operator=(const MonteCarloTreeSearchCache&) = delete;

	void addToExpansion(ExpansionData&& data);
	void convertToNeuralInput();
	void expand(NeuralNetwork* net); // convertToNeuralInput must be called before calling this
	long long getMemSize() const;
	void clear();

private:
	int addToInput(torch::Tensor inputTensor);
	void calculateOutput(NeuralNetwork* net);
	std::pair<torch::Tensor, torch::Tensor> getOutput(size_t index);

	torch::Tensor m_input;
	torch::Tensor m_outputProbabilities;
	torch::Tensor m_outputValues;
	torch::DeviceType m_device;
	size_t m_inputSize = 0;
	size_t m_outputSize = 0;
	std::map<std::string, std::vector<std::pair<int, float>>> m_probabilities;
	std::map<std::string, float> m_values;
	std::set<ExpansionData> toExpand;
	Game* m_game;
};

class MonteCarloTreeSearch
{
public:
	MonteCarloTreeSearch(int actionCount, torch::DeviceType device, Game* game, MonteCarloTreeSearchCache* cache = nullptr, float cpuct = 1.0);
	void search(int count, const std::string& strState, NeuralNetwork* net, int currentPlayer);
	float search(const std::string& strState, NeuralNetwork* net, int currentPlayer);
	bool startSearchWithoutExpansion(const std::string& strState, int currentPlayer, int count);
	bool expandAndContinueSearchWithoutExpansion(const std::string& strState, int currentPlayer);
	std::vector<std::pair<int, float>> getProbabilities(const std::string& state, float temperature = 1.0);
	long long getMemSize() const;

private:
	float searchWithoutExpansion(std::string strState, int currentPlayer, bool* expansionNeeded);
	bool runMultipleSearches(const std::string& strState, int currentPlayer);
	void backpropagateValue(float value);
	void deferredExpansion();
	float expandNewEncounteredState(const std::string& strState, int currentPlayer, NeuralNetwork* net);
	int getActionWithHighestUpperConfidenceBound(const std::string* statePtr, int currentPlayer);
	float calculateUpperConfidenceBound(const std::string* statePtr, int action, float probability, unsigned int stateVisitCountSum);
	unsigned int getVisitCountSum(const std::string* statePtr) const;

	int m_actionCount = -1;
	int m_mctsCount = 0;
	torch::DeviceType m_device = torch::kCPU;
	float m_cpuct = -1.0;
	MonteCarloTreeSearchCache* m_cache;
	Game* m_game = nullptr;
	std::set<std::string> m_visited; // Only save the actual state string in this set -> this saves us some space
	// Use boost flat_map, this reduces memory consumption quite a bit
	boost::container::flat_map<const std::string*, boost::container::flat_map<int, int>> m_visitCount;
	boost::container::flat_map<const std::string*, boost::container::flat_map<int, float>> m_qValues;
	boost::container::flat_map<const std::string*, std::vector<std::pair<int, float>>> m_probabilities;
	boost::container::flat_set<const std::string*> m_loopDetection;

	struct BackPropData
	{
		BackPropData(std::string state, int player) : state(std::move(state)), player(player) {};

		std::string state;
		int player;
		int bestAction = -1;
	};
	std::vector<BackPropData> m_backProp;
};

#endif //DEEPREINFORCEMENTLEARNING_MONTECARLOTREESEARCH_H

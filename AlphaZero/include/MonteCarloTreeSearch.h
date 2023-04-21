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

	MonteCarloTreeSearchCache(torch::DeviceType device, Game* game) : m_device(device), m_game(game) {}
	MonteCarloTreeSearchCache(const MonteCarloTreeSearchCache&) = delete;
	MonteCarloTreeSearchCache& operator=(const MonteCarloTreeSearchCache&) = delete;

	void addToExpansion(const ExpansionData& data)
	{
		toExpand.insert(data);
	}

	void convertToNeuralInput()
	{
		for (const auto& state : toExpand)
			addToInput(m_game->convertStateToNeuralNetInput(state.state, state.currentPlayer));
	}

	void expand(NeuralNetwork* net) // convertToNeuralInput must be called before calling this
	{
		calculateOutput(net);

		size_t counter = 0;
		for (const auto& state : toExpand)
		{
			auto [iterator, flag] = encountered.emplace(state.state);
			auto statePtr = &(*iterator);

			auto [val, probs] = getOutput(counter++);

			m_values[statePtr] = *(val[0].data_ptr<float>());

			for (const auto& move : m_game->getAllPossibleMoves(state.state, state.currentPlayer))
				m_probabilities[statePtr].emplace_back(move, *(probs[move].data_ptr<float>()));
		}
		toExpand.clear();
	}

	long long printMemsize() const;

private:
	int addToInput(torch::Tensor inputTensor)
	{
		if (m_input.numel() == 0)
			m_input = inputTensor;
		else
			m_input = torch::cat({ m_input, inputTensor }, 0);
		return m_inputSize++;
	}

	void calculateOutput(NeuralNetwork* net)
	{
		if (m_inputSize == 0)
			return;

		m_input = m_input.to(m_device);
		auto rawOutput = net->calculate(m_input);
		m_outputSize = m_inputSize;
		m_inputSize = 0;
		m_input = torch::Tensor{};

		m_outputValues = std::get<0>(rawOutput).detach().to(torch::kCPU);
		m_outputProbabilities = std::get<1>(rawOutput).detach().to(torch::kCPU);
	}

	std::pair<torch::Tensor, torch::Tensor> getOutput(size_t index)
	{
		assert(index < m_outputSize);

		return { m_outputValues[index], m_outputProbabilities[index] };
	}

	torch::Tensor m_input;
	torch::Tensor m_outputProbabilities;
	torch::Tensor m_outputValues;
	torch::DeviceType m_device;
	size_t m_inputSize = 0;
	size_t m_outputSize = 0;
	std::set<std::string> encountered; // Only save the actual state string in this set -> this saves us some space
	std::map<const std::string*, std::vector<std::pair<int, float>>> m_probabilities;
	std::map<const std::string*, float> m_values;
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
	torch::Tensor getExpansionNeuralNetInput(Game* game) const;
	void printMemsize() const;
	long long getMemSize() const;

private:
	float searchWithoutExpansion(std::string strState, int currentPlayer, bool* expansionNeeded);
	bool runMultipleSearches(const std::string& strState, int currentPlayer);
	void backpropagateValue(float value);
	void deferredExpansion();
	float expandNewEncounteredState(const std::string& strState, int currentPlayer, NeuralNetwork* net);
	int getActionWithHighestUpperConfidenceBound(const std::string* statePtr, int currentPlayer);
	float calculateUpperConfidenceBound(const std::string* statePtr, int action, float probability);

	int m_actionCount = -1;
	int m_mctsCount = 0;
	torch::DeviceType m_device = torch::kCPU;
	float m_cpuct = -1.0;
	MonteCarloTreeSearchCache* m_cache;
	Game* m_game = nullptr;
	std::set<const std::string*> m_loopDetection;
	std::set<const std::string*> m_visited;
	/*
	m_visitCountSum is not needed if we sum up the map in m_visitCount,
	however having a separate map for this is better performance wise
	*/
	std::map<const std::string*, int> m_visitCountSum;
	std::map<const std::string*, std::map<int, int>> m_visitCount;
	std::map<const std::string*, std::map<int, float>> m_qValues;
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

#ifndef DEEPREINFORCEMENTLEARNING_MONTECARLOTREESEARCHTEMPLATE_H
#define DEEPREINFORCEMENTLEARNING_MONTECARLOTREESEARCHTEMPLATE_H

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
#include "NeuralNetworks/NeuralNetwork.h"

template <typename BoardT, typename GameT, bool mockExpansion = true>
class MonteCarloTreeSearchCacheT
{
public:
	friend class MonteCarloTreeSearch;

	struct ExpansionDataT
	{
		BoardT state;
		int currentPlayer;

		friend bool operator<(const ExpansionDataT& lhs, const ExpansionDataT& rhs)
		{
			return lhs.state < rhs.state;
		}
	};

	MonteCarloTreeSearchCacheT(torch::DeviceType device, GameT* game)
		: m_device(device)
		, m_game(game)
	{
	}
	MonteCarloTreeSearchCacheT(const MonteCarloTreeSearchCache&) = delete;
	MonteCarloTreeSearchCacheT& operator=(const MonteCarloTreeSearchCache&) = delete;

	void addToExpansion(ExpansionDataT&& data)
	{
		toExpand.emplace(std::move(data));
	}

	void convertToNeuralInput()
	{
		torch::NoGradGuard no_grad;
		for (const auto& state : toExpand)
			addToInput(state);
	}

	void expand(NeuralNetwork* net) // convertToNeuralInput must be called before calling this
	{
		torch::NoGradGuard no_grad;
		m_values.clear();
		m_probabilities.clear();

		if constexpr (mockExpansion)
		{
			size_t counter = 0;
			for (const auto& state : toExpand)
			{
				m_values[state.state] = ALZ::getRandomNumber(-1.0, 1.0);

				for (const auto& move : m_game->getAllPossibleMoves(state.state, state.currentPlayer))
					m_probabilities[state.state].emplace_back(move, ALZ::getRandomNumber(0.0, 1.0));
			}
			m_currentInputSize = 0;
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

private:
	int addToInput(ExpansionDataT data)
	{
		if (m_maxSize == 0)
		{
			m_input = m_game->convertStateToNeuralNetInput(data.state, data.currentPlayer);
			m_maxSize++;
		}
		else if (m_currentInputSize >= m_maxSize)
		{
			torch::Tensor tens = m_game->convertStateToNeuralNetInput(data.state, data.currentPlayer);
			m_input = torch::cat({ m_input, tens }, 0);
			m_maxSize++;
		}
		else
		{
			m_game->convertStateToNeuralNetInput(data.state, data.currentPlayer, m_input[m_currentInputSize]);
		}

		return m_currentInputSize++;
	}

	void calculateOutput(NeuralNetwork* net)
	{
		if (m_currentInputSize == 0)
			return;

		auto device_input = m_input.to(m_device);
		auto rawOutput = net->calculate(device_input);
		m_outputSize = m_currentInputSize;
		m_currentInputSize = 0;

		m_outputValues = std::get<0>(rawOutput).to(torch::kCPU);
		m_outputProbabilities = std::get<1>(rawOutput).to(torch::kCPU);
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
	size_t m_currentInputSize = 0;
	size_t m_maxSize = 0;
	size_t m_outputSize = 0;
	std::map<BoardT, std::vector<std::pair<int, float>>> m_probabilities;
	std::map<BoardT, float> m_values;
	std::set<ExpansionDataT> toExpand;
	GameT* m_game;
};

template <typename BoardT, typename GameT>
class MonteCarloTreeSearchT
{
public:
	MonteCarloTreeSearchT(MonteCarloTreeSearchCache* cache, GameT* game, torch::DeviceType device, float cpuct = 1.0)
		: m_cache(cache)
		, m_game(game)
		, m_device(device)
		, m_actionCount(game->getActionCount())
		, m_cpuct(cpuct)
	{
	}
	void search(int count, const std::string& strState, NeuralNetwork* net, int currentPlayer);
	float search(const std::string& strState, NeuralNetwork* net, int currentPlayer);
	bool startSearchWithoutExpansion(const std::string& strState, int currentPlayer, int count);
	bool expandAndContinueSearchWithoutExpansion(const std::string& strState, int currentPlayer);
	std::vector<std::pair<int, float>> getProbabilities(const std::string& state, float temperature = 1.0);

private:
	float searchWithoutExpansion(std::string strState, int currentPlayer, bool* expansionNeeded);
	bool runMultipleSearches(const std::string& strState, int currentPlayer);
	void backpropagateValue(float value);
	void deferredExpansion();
	float expandNewEncounteredState(const std::string& strState, int currentPlayer, NeuralNetwork* net);
	int getActionWithHighestUpperConfidenceBound(const std::string* statePtr, int currentPlayer) 
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

	float calculateUpperConfidenceBound(const std::string* statePtr, int action, float probability, unsigned int stateVisitCountSum) 
	{
		const float buf = sqrt(stateVisitCountSum) / (1 + m_visitCount[statePtr][action]);

		return m_qValues[statePtr][action] + m_cpuct * probability * buf;
	}

	unsigned int getVisitCountSum(const std::string* statePtr) const 
	{
		assert(statePtr);
		unsigned int countSum = 0;
		const auto iter = m_visitCount.find(statePtr);
		if (iter == m_visitCount.end())
			return 0;
		for (const auto& [action, visitCount] : iter->second)
			countSum += visitCount;

		return countSum;
	};

	struct BackPropData
	{
		BackPropData(BoardT board, int player)
			: state(std::move(state))
			, player(player)
		{
		};
		BoardT state;
		int player;
		int bestAction = -1;
	};

	MonteCarloTreeSearchCacheT<BoardT, GameT>* m_cache;
	GameT* m_game = nullptr;
	torch::DeviceType m_device = torch::kCPU;
	int m_actionCount = -1;
	float m_cpuct = -1.0;
	int m_mctsCount = 0;
	std::set<BoardT> m_visited; // Only save the actual state string in this set -> this saves us some space
	// Use boost flat_map, this reduces memory consumption quite a bit
	std::map<const BoardT*, boost::container::flat_map<int, int>> m_visitCount;
	std::map<const BoardT*, boost::container::flat_map<int, float>> m_qValues;
	std::map<const BoardT*, std::vector<std::pair<int, float>>> m_probabilities;
	std::set<const BoardT*> m_loopDetection;
	std::vector<BackPropData> m_backProp;
};


#endif //DEEPREINFORCEMENTLEARNING_MONTECARLOTREESEARCHTEMPLATE_H

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

template <typename Board, typename GameT>
class MonteCarloTreeSearchCacheT
{
public:
	friend class MonteCarloTreeSearch;

	struct ExpansionDataT
	{
		Board state;
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
		//if (MOCK_EXPAND)
		//	return;
		for (const auto& state : toExpand)
			addToInput(state);
	}

	void expand(NeuralNetwork* net) // convertToNeuralInput must be called before calling this
	{
		torch::NoGradGuard no_grad;
		m_values.clear();
		m_probabilities.clear();

		//if (MOCK_EXPAND)
		//{
		//	size_t counter = 0;
		//	for (const auto& state : toExpand)
		//	{
		//		m_values[state.state] = ALZ::getRandomNumber(-1.0, 1.0);

		//		for (const auto& move : m_game->getAllPossibleMoves(state.state, state.currentPlayer))
		//			m_probabilities[state.state].emplace_back(move, ALZ::getRandomNumber(0.0, 1.0));
		//	}
		//}
		//else
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
	std::map<Board, std::vector<std::pair<int, float>>> m_probabilities;
	std::map<Board, float> m_values;
	std::set<ExpansionDataT> toExpand;
	GameT* m_game;
};


#endif //DEEPREINFORCEMENTLEARNING_MONTECARLOTREESEARCHTEMPLATE_H

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

template <typename GameState, typename Game, bool mockExpansion>
class MonteCarloTreeSearch;

template <typename GameState, typename Game, bool mockExpansion = false>
class MonteCarloTreeSearchCache
{
public:
	friend class MonteCarloTreeSearch<GameState, Game, mockExpansion>;

	struct ExpansionDataT
	{
		GameState state;
		int currentPlayer;

		friend bool operator<(const ExpansionDataT& lhs, const ExpansionDataT& rhs)
		{
			return lhs.state < rhs.state;
		}
	};

	MonteCarloTreeSearchCache(torch::DeviceType device, Game* game)
		: m_device(device)
		, m_game(game)
	{
	}
	MonteCarloTreeSearchCache(const MonteCarloTreeSearchCache&) = delete;
	MonteCarloTreeSearchCache& operator=(const MonteCarloTreeSearchCache&) = delete;

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
				const auto allPossibleMoves = m_game->getAllPossibleMoves(state.state, state.currentPlayer);
				m_values[state.state] = 1.0 / allPossibleMoves.size();

				for (const auto& move : allPossibleMoves)
					m_probabilities[state.state].emplace_back(move, 1.0 / allPossibleMoves.size());
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
				auto probsAccessor = probs.template accessor<float, 1>();

				m_values[state.state] = val[0].template item<float>();

				for (const auto& move : m_game->getAllPossibleMoves(state.state, state.currentPlayer))
					m_probabilities[state.state].emplace_back(move, probsAccessor[move]);
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
	std::map<GameState, std::vector<std::pair<int, float>>> m_probabilities;
	std::map<GameState, float> m_values;
	std::set<ExpansionDataT> toExpand;
	Game* m_game;
};

template <typename GameState, typename Game, bool mockExpansion = false>
class MonteCarloTreeSearch
{
public:
	struct StateInformation
	{
		boost::container::flat_map<int, int> m_visitCount;
		boost::container::flat_map<int, float> m_qValues;
		std::vector<std::pair<int, float>> m_probabilities;
	};

	MonteCarloTreeSearch(MonteCarloTreeSearchCache<GameState, Game, mockExpansion>* cache, Game* game, torch::DeviceType device, float cpuct = 1.0)
		: m_cache(cache)
		, m_game(game)
		, m_device(device)
		, m_actionCount(game->getActionCount())
		, m_cpuct(cpuct)
	{
	}

	void search(size_t count, const GameState& state, NeuralNetwork* net, int currentPlayer)
	{
		for (size_t i = 0; i < count; i++)
		{
			search(state, net, currentPlayer);
			m_loopDetection.clear();
		}
	}

	float search(const GameState& state, NeuralNetwork* net, int currentPlayer)
	{
		m_backProp.clear();
		bool expansionNeeded = false;
		const float value = searchWithoutExpansion(state, currentPlayer, &expansionNeeded);
		// This must not happen in self play, however some Unit-tests might trigger this assertion -> therefore deactivated for now
		// assert(!m_backProp.empty()); 

		if (expansionNeeded)
			return -expand(net);

		backpropagateValue(value);
		return value;
	}

	bool startSearchWithoutExpansion(const GameState& state, int currentPlayer, int count)
	{
		m_mctsCount = count;

		return runMultipleSearches(state, currentPlayer);
	}

	bool expandAndContinueSearchWithoutExpansion(const GameState& state, int currentPlayer)
	{
		finishExpansion();

		return runMultipleSearches(state, currentPlayer);
	}

	std::vector<std::pair<int, float>> getProbabilities(const GameState& state, float temperature = 1.0) const
	{
		auto stateIter = m_visitedState.find(state);
		const auto& currentStateInfo = stateIter->second;
		assert(stateIter != m_visitedState.end());

		const int countSum = getVisitCountSum(currentStateInfo);
		std::vector<std::pair<int, float>> probs;
		probs.reserve(currentStateInfo.m_probabilities.size());

		for (const auto& [action, visitCount] : currentStateInfo.m_visitCount)
		{
			const float probability = static_cast<float>(pow(visitCount, 1.f / temperature)) / countSum;
			probs.emplace_back(action, probability);
		}

		return probs;
	}

private:
	float searchWithoutExpansion(GameState gameState, int currentPlayer, bool* expansionNeeded)
	{
		while (true) // Iterate until we reach a "leaf state" (a state not yet expanded or a game over state)
		{
			if (m_game->isGameOver(gameState))
				return m_game->gameOverReward(gameState, currentPlayer);

			m_backProp.emplace_back(std::move(gameState), currentPlayer);
			const auto& currentState = m_backProp.back().state;

			auto currentStateItr = m_visitedState.find(currentState);
			if (currentStateItr == m_visitedState.end())
			{
				m_cache->addToExpansion({ currentState, currentPlayer });
				*expansionNeeded = true;
				return 0;
			}

			m_loopDetection.emplace(&(currentStateItr->first));
			int bestAction = getActionWithHighestUpperConfidenceBound(currentStateItr->second, currentPlayer);
			m_backProp.back().bestAction = bestAction;
			gameState = m_game->makeMove(currentState, bestAction, currentPlayer);

			auto nextStateItr = m_visitedState.find(gameState);
			if (nextStateItr != m_visitedState.end())
			{
				if (m_loopDetection.find(&(nextStateItr->first)) != m_loopDetection.end())
					return 0;
			}
			currentPlayer = m_game->getNextPlayer(currentPlayer);
		}
	}

	bool runMultipleSearches(const GameState& strState, int currentPlayer)
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

	void backpropagateValue(float value)
	{
		while (!m_backProp.empty())
		{
			value = -value;
			auto& backProp = m_backProp.back();
			auto& state = backProp.state;
			auto& currentStateInfo = m_visitedState.find(state)->second;
			auto bestAction = backProp.bestAction;
			currentStateInfo.m_qValues[bestAction] = (currentStateInfo.m_visitCount[bestAction] * currentStateInfo.m_qValues[bestAction] + value) / (currentStateInfo.m_visitCount[bestAction] + 1);
			currentStateInfo.m_visitCount[bestAction] += 1;

			m_backProp.pop_back();
		}
	}

	float expand(NeuralNetwork* net)
	{
		m_cache->addToExpansion({ m_backProp.back().state, m_backProp.back().player });
		m_cache->convertToNeuralInput();
		m_cache->expand(net);

		return finishExpansion();
	}

	float finishExpansion()
	{
		assert(!m_backProp.empty());
		const auto& currentState = m_backProp.back().state;
		const float value = m_cache->m_values[currentState];
		const auto [iter, success] = m_visitedState.emplace(std::move(currentState), StateInformation());
		iter->second.m_probabilities = m_cache->m_probabilities[iter->first];

		m_backProp.pop_back();
		backpropagateValue(value);

		return value;
	}

	int getActionWithHighestUpperConfidenceBound(const StateInformation& stateInfo, int currentPlayer) const
	{
		float maxUtility = std::numeric_limits<float>::lowest();
		int bestAction = -1;

		const auto stateVisitCountSum = getVisitCountSum(stateInfo);
		if (stateVisitCountSum == 0)
			return stateInfo.m_probabilities.front().first;

		for (const auto& [action, probability] : stateInfo.m_probabilities)
		{
			float utility = calculateUpperConfidenceBound(stateInfo, action, probability, stateVisitCountSum);

			if (utility > maxUtility)
			{
				maxUtility = utility;
				bestAction = action;
			}
		}
		assert(bestAction != -1);

		return bestAction;
	}

	float calculateUpperConfidenceBound(const StateInformation& stateInfo, int action, float probability, unsigned int stateVisitCountSum) const
	{
		float stateVisitCount = 0.f;
		float stateQvalue = 0.f;
		auto visitCountIter = stateInfo.m_visitCount.find(action);
		if (visitCountIter != stateInfo.m_visitCount.end()) 
		{
			stateVisitCount = visitCountIter->second;
			stateQvalue = stateInfo.m_qValues.at(action);
		}

		const float buf = sqrt(stateVisitCountSum) / (1.0 + stateVisitCount);

		return stateQvalue + m_cpuct * probability * buf;
	}

	unsigned int getVisitCountSum(const StateInformation& stateInfo) const
	{
		unsigned int countSum = 0;
		for (const auto& [action, visitCount] : stateInfo.m_visitCount)
			countSum += visitCount;

		return countSum;
	};

	struct BackPropData
	{
		BackPropData(GameState board, int player)
			: state(std::move(board))
			, player(player)
		{
		};
		GameState state;
		int player;
		int bestAction = -1;
	};

	MonteCarloTreeSearchCache<GameState, Game, mockExpansion>* m_cache;
	Game* m_game = nullptr;
	torch::DeviceType m_device = torch::kCPU;
	int m_actionCount = -1;
	float m_cpuct = -1.0;
	int m_mctsCount = 0;

	std::map<GameState, StateInformation> m_visitedState;
	// Use boost flat_map, this reduces memory consumption quite a bit
	std::set<const GameState*> m_loopDetection;
	std::vector<BackPropData> m_backProp;
};

#endif //DEEPREINFORCEMENTLEARNING_MONTECARLOTREESEARCHTEMPLATE_H

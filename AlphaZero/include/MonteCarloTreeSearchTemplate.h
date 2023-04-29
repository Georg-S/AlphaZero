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

template <typename GameStateT, typename GameT, bool mockExpansion>
class MonteCarloTreeSearchT;

template <typename GameStateT, typename GameT, bool mockExpansion = false>
class MonteCarloTreeSearchCacheT
{
public:
	friend class MonteCarloTreeSearchT<GameStateT, GameT, mockExpansion>;

	struct ExpansionDataT
	{
		GameStateT state;
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
	MonteCarloTreeSearchCacheT(const MonteCarloTreeSearchCacheT&) = delete;
	MonteCarloTreeSearchCacheT& operator=(const MonteCarloTreeSearchCacheT&) = delete;

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
	std::map<GameStateT, std::vector<std::pair<int, float>>> m_probabilities;
	std::map<GameStateT, float> m_values;
	std::set<ExpansionDataT> toExpand;
	GameT* m_game;
};

template <typename GameStateT, typename GameT, bool mockExpansion = false>
class MonteCarloTreeSearchT
{
public:
	MonteCarloTreeSearchT(MonteCarloTreeSearchCacheT<GameStateT, GameT, mockExpansion>* cache, GameT* game, torch::DeviceType device, float cpuct = 1.0)
		: m_cache(cache)
		, m_game(game)
		, m_device(device)
		, m_actionCount(game->getActionCount())
		, m_cpuct(cpuct)
	{
	}

	void search(size_t count, const GameStateT& state, NeuralNetwork* net, int currentPlayer)
	{
		for (size_t i = 0; i < count; i++)
		{
			search(state, net, currentPlayer);
			m_loopDetection.clear();
		}
	}

	float search(const GameStateT& state, NeuralNetwork* net, int currentPlayer)
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

	bool startSearchWithoutExpansion(const GameStateT& state, int currentPlayer, int count)
	{
		m_mctsCount = count;

		return runMultipleSearches(state, currentPlayer);
	}

	bool expandAndContinueSearchWithoutExpansion(const GameStateT& state, int currentPlayer)
	{
		finishExpansion();

		return runMultipleSearches(state, currentPlayer);
	}

	std::vector<std::pair<int, float>> getProbabilities(const GameStateT& state, float temperature = 1.0)
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

private:
	float searchWithoutExpansion(GameStateT gameState, int currentPlayer, bool* expansionNeeded)
	{
		while (true) // Iterate until we reach a "leaf state" (a state not yet expanded or a game over state)
		{
			if (m_game->isGameOver(gameState))
				return m_game->gameOverReward(gameState, currentPlayer);

			m_backProp.emplace_back(std::move(gameState), currentPlayer);
			const auto& currentState = m_backProp.back().state;

			auto currentStateItr = m_visited.find(currentState);
			if (currentStateItr == m_visited.end())
			{
				m_cache->addToExpansion({ currentState, currentPlayer });
				*expansionNeeded = true;
				return 0;
			}

			auto statePtr = &(*currentStateItr);
			m_loopDetection.emplace(statePtr);
			int bestAction = getActionWithHighestUpperConfidenceBound(statePtr, currentPlayer);
			m_backProp.back().bestAction = bestAction;
			gameState = m_game->makeMove(currentState, bestAction, currentPlayer);

			auto nextStateItr = m_visited.find(gameState);
			if (nextStateItr != m_visited.end())
			{
				if (m_loopDetection.find(&(*nextStateItr)) != m_loopDetection.end())
					return 0;
			}
			currentPlayer = m_game->getNextPlayer(currentPlayer);
		}
	}

	bool runMultipleSearches(const GameStateT& strState, int currentPlayer)
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
			auto statePtr = &(*m_visited.find(state));
			auto bestAction = backProp.bestAction;
			m_qValues[statePtr][bestAction] = (m_visitCount[statePtr][bestAction] * m_qValues[statePtr][bestAction] + value) / (m_visitCount[statePtr][bestAction] + 1);
			m_visitCount[statePtr][bestAction] += 1;
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
		const auto [iter, success] = m_visited.emplace(std::move(m_backProp.back().state));
		const auto statePtr = &(*iter);
		const float value = m_cache->m_values[*statePtr];
		m_probabilities[statePtr] = m_cache->m_probabilities[*statePtr];
		assert(!m_probabilities[statePtr].empty());

		m_backProp.pop_back();
		backpropagateValue(value);

		return value;
	}

	int getActionWithHighestUpperConfidenceBound(const GameStateT* statePtr, int currentPlayer)
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

	float calculateUpperConfidenceBound(const GameStateT* statePtr, int action, float probability, unsigned int stateVisitCountSum)
	{
		const float buf = sqrt(stateVisitCountSum) / (1 + m_visitCount[statePtr][action]);

		return m_qValues[statePtr][action] + m_cpuct * probability * buf;
	}

	unsigned int getVisitCountSum(const GameStateT* statePtr) const
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
		BackPropData(GameStateT board, int player)
			: state(std::move(board))
			, player(player)
		{
		};
		GameStateT state;
		int player;
		int bestAction = -1;
	};

	MonteCarloTreeSearchCacheT<GameStateT, GameT, mockExpansion>* m_cache;
	GameT* m_game = nullptr;
	torch::DeviceType m_device = torch::kCPU;
	int m_actionCount = -1;
	float m_cpuct = -1.0;
	int m_mctsCount = 0;
	std::set<GameStateT> m_visited; // Only save the actual state string in this set -> this saves us some space
	// Use boost flat_map, this reduces memory consumption quite a bit
	std::map<const GameStateT*, boost::container::flat_map<int, int>> m_visitCount;
	std::map<const GameStateT*, boost::container::flat_map<int, float>> m_qValues;
	std::map<const GameStateT*, std::vector<std::pair<int, float>>> m_probabilities;
	std::set<const GameStateT*> m_loopDetection;
	std::vector<BackPropData> m_backProp;
};

#endif //DEEPREINFORCEMENTLEARNING_MONTECARLOTREESEARCHTEMPLATE_H

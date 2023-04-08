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

class NeuralNetInputBuffer 
{
public:
	NeuralNetInputBuffer() = default;
	NeuralNetInputBuffer(const NeuralNetInputBuffer&) = delete;
	NeuralNetInputBuffer& operator=(const NeuralNetInputBuffer&) = delete;

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

		auto rawOutput = net->calculate(m_input);
		m_outputSize = m_inputSize;
		m_inputSize = 0;
		m_input = torch::Tensor{};

		m_outputValues = std::get<0>(rawOutput).detach().to(torch::kCPU);
		m_outputProbabilities = std::get<1>(rawOutput).detach().to(torch::kCPU);
	}

	std::pair<torch::Tensor,torch::Tensor> getOutput(size_t index)
	{
		assert(index < m_outputSize);

		return { m_outputValues[index], m_outputProbabilities[index] };
	}

private:
	torch::Tensor m_input;
	torch::Tensor m_outputProbabilities;
	torch::Tensor m_outputValues;
	size_t m_inputSize = 0;
	size_t m_outputSize = 0;
};

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
	void backpropagateValue(float value);
	void deferredExpansion(torch::Tensor valueTens, torch::Tensor probabilities);
	bool runMultipleSearches(const std::string& strState, Game* game, int currentPlayer);
	bool specialSearch(const std::string& strState, Game* game, int currentPlayer, int count);
	bool continueSpecialSearch(const std::string& strState, Game* game, int currentPlayer, torch::Tensor valueTens, torch::Tensor probabilities);
	void specialSearch(const std::string& strState, NeuralNetwork* net, Game* game, int currentPlayer, torch::DeviceType device = torch::kCPU);
	float searchWithoutExpansion(const std::string& strState, Game* game, int currentPlayer, bool* expansionNeeded);
	float multiThreadedSearch(std::string strState, Game* game, int currentPlayer,
		MultiThreadingNeuralNetManager* threadingManager,
		torch::DeviceType device = torch::kCPU);
	std::vector<float> getProbabilities(const std::string& state, float temperature = 1.0);
	void clearAll();
	void fillQValuesAndVisitCount(const std::string& state);
	torch::Tensor getExpansionNeuralNetInput(Game* game, torch::DeviceType device) const;

private:
	float expandNewEncounteredState(const std::string& strState, int currentPlayer, Game* game, NeuralNetwork* net, torch::DeviceType device);
	float multiThreadedExpandNewState(const std::string& strState, int currentPlayer, Game* game,
		MultiThreadingNeuralNetManager* threadingManager, torch::DeviceType device);
	int getActionWithHighestUpperConfidenceBound(const std::string& strState, int currentPlayer, Game* game);
	float calculateUpperConfidenceBound(const std::string& strState, int action);

	int m_actionCount = -1;
	float m_cpuct = -1.0;
	int m_mctsCount = 0;
	std::map<std::string, bool> m_loopDetection;
	std::map<std::string, bool> m_visited;
	std::map<std::string, std::vector<int>> m_visitCount;
	std::map<std::string, std::vector<float>> m_qValues;
	std::map<std::string, torch::Tensor> m_probabilities;
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

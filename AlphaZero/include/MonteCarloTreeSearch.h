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
	NeuralNetInputBuffer(torch::DeviceType device) : m_device(device) {}
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

		m_input = m_input.to(m_device);
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
	torch::DeviceType m_device;
	size_t m_inputSize = 0;
	size_t m_outputSize = 0;
};

class MonteCarloTreeSearch
{
public:
	MonteCarloTreeSearch(int actionCount, torch::DeviceType device, float cpuct = 1.0);
	void search(int count, const std::string& strState, NeuralNetwork* net, Game* game, int currentPlayer);
	float search(const std::string& strState, NeuralNetwork* net, Game* game, int currentPlayer);
	bool startSearchWithoutExpansion(const std::string& strState, Game* game, int currentPlayer, int count);
	bool expandAndContinueSearchWithoutExpansion(const std::string& strState, Game* game, int currentPlayer, torch::Tensor valueTens, torch::Tensor probabilities);
	std::vector<float> getProbabilities(const std::string& state, float temperature = 1.0);
	torch::Tensor getExpansionNeuralNetInput(Game* game) const;

private:
	float searchWithoutExpansion(std::string strState, Game* game, int currentPlayer, bool* expansionNeeded);
	bool runMultipleSearches(const std::string& strState, Game* game, int currentPlayer);
	void backpropagateValue(float value);
	void deferredExpansion(torch::Tensor valueTens, torch::Tensor probabilities, Game* game);
	float expandNewEncounteredState(const std::string& strState, int currentPlayer, Game* game, NeuralNetwork* net);
	int getActionWithHighestUpperConfidenceBound(const std::string* statePtr, int currentPlayer, Game* game);
	float calculateUpperConfidenceBound(const std::string* statePtr, int action, float probability);

	int m_actionCount = -1;
	int m_mctsCount = 0;
	torch::DeviceType m_device = torch::kCPU;
	float m_cpuct = -1.0;
	std::set<std::string> m_visited; // Only save the actual state string in this set -> this saves us some space
	std::set<const std::string*> m_loopDetection;
	std::map<const std::string*, std::map<int,int>> m_visitCount;
	std::map<const std::string*, std::map<int,float>> m_qValues;
	std::map<const std::string*, std::vector<std::pair<int, float>>> m_probabilities;
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

#ifndef DEEPREINFORCEMENTLEARNING_DEFAULTNEURALNET_H
#define DEEPREINFORCEMENTLEARNING_DEFAULTNEURALNET_H

#include "NeuralNetwork.h"
#include "CutDownAlphaGoZeroNet.h"
#include "AlphaGoZeroNet.h"

class DefaultNeuralNet : public NeuralNetwork
{
public:
	DefaultNeuralNet(int64_t numPlanes, int64_t width, int64_t height, int64_t numOutputs, torch::DeviceType device = torch::kCPU);
	DefaultNeuralNet(int64_t numPlanes, int64_t width, int64_t height, int64_t numOutputs, std::string fileName, torch::DeviceType device = torch::kCPU);
	~DefaultNeuralNet() = default;
	void load(std::string fileName) override;
	void save(std::string fileName) override;
	std::tuple<torch::Tensor, torch::Tensor> calculate(const torch::Tensor& input) override;
	void copyNetFrom(NeuralNetwork* copySource) override;
	void setLearningRate(float learningRate) override;
	void training(torch::Tensor& val, torch::Tensor& probs, torch::Tensor& probsTarget, torch::Tensor& valueTarget) override;
	/// Sets the network to production mode (interference)
	/**
	*  This is needed because some parts of a neural network differ from training to interference.
	*  E.g. In "Training Mode" the result of a batch normalization depends on the batch size of the input
	*  however in interference mode, the batch size has no impact on the batch normalization at all.
	*/
	void setToEval();
	void setToTraining();

private:
	CutDownAlphaGoZeroNet net = nullptr;
	float learningRate = 0.01;
};


#endif //DEEPREINFORCEMENTLEARNING_DEFAULTNEURALNET_H

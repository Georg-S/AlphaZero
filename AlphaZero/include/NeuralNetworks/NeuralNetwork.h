#ifndef DEEPREINFORCEMENTLEARNING_NEURALNETWORK_H
#define DEEPREINFORCEMENTLEARNING_NEURALNETWORK_H
#include <string>
#include <tuple>
// Libtorch has many warnings which clutter the output, so we ignore them
#pragma warning(push, 0)
#include <torch/torch.h>
#pragma warning(pop)

class NeuralNetwork
{
public:
	virtual ~NeuralNetwork() = default;
	virtual std::tuple<torch::Tensor, torch::Tensor> calculate(const torch::Tensor& input) = 0;
	virtual void save(std::string) = 0;
	virtual void load(std::string) = 0;
	virtual void training(torch::Tensor& val, torch::Tensor& probs, torch::Tensor& probsTarget, torch::Tensor& valueTarget) = 0;
	virtual void setLearningRate(float learningRate) = 0;
	virtual void copyNetFrom(NeuralNetwork* copySource) = 0;
};

#endif //DEEPREINFORCEMENTLEARNING_NEURALNETWORK_H

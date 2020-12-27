#ifndef DEEPREINFORCEMENTLEARNING_NEURALNETWORK_H
#define DEEPREINFORCEMENTLEARNING_NEURALNETWORK_H
#include <string>
#include <tuple>
#include <torch/torch.h>

class NeuralNetwork 
{
public:
	virtual std::tuple<torch::Tensor, torch::Tensor> calculate(const torch::Tensor& input) = 0;
	virtual void save(std::string) = 0;
	virtual void load(std::string) = 0;
	virtual void training(torch::Tensor& val, torch::Tensor& probs, torch::Tensor& probsTarget, torch::Tensor& valueTarget) = 0;
	virtual void setLearningRate(float learningRate) = 0;
	virtual void copyNetFrom(NeuralNetwork* copySource) = 0;
};

#endif //DEEPREINFORCEMENTLEARNING_NEURALNETWORK_H

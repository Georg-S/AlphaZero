#ifndef DEEPREINFORCEMENTLEARNING_DEFAULTNORESIDUAL_H
#define DEEPREINFORCEMENTLEARNING_DEFAULTNORESIDUAL_H

#include "NeuralNetwork.h"
#include "NoResidualNNImpl.h"

class DefaultNoResidual : public NeuralNetwork {
public:
    DefaultNoResidual(int64_t numPlanes, int64_t width, int64_t height, int64_t numOutputs, torch::DeviceType device = torch::kCPU);
    DefaultNoResidual(int64_t numPlanes, int64_t width, int64_t height, int64_t numOutputs, std::string fileName, torch::DeviceType device = torch::kCPU);
    ~DefaultNoResidual();

    void load(std::string fileName) override;
    void save(std::string fileName) override;
    std::tuple<torch::Tensor, torch::Tensor> calculate(const torch::Tensor &input) override;
    void copyNetFrom(NeuralNetwork *copySource) override;
    void setLearningRate(const float &learningRate) override;
    void training(torch::Tensor &val, torch::Tensor &probs, torch::Tensor &probsTarget, torch::Tensor &valueTarget) override;

private:
    NoResidualNN net = nullptr;
    float learningRate = 0.01;
};


#endif //DEEPREINFORCEMENTLEARNING_DEFAULTNORESIDUAL_H

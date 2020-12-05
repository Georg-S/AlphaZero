#include "NeuralNetworks/DefaultNoResidual.h"

DefaultNoResidual::DefaultNoResidual(int64_t numPlanes, int64_t width, int64_t height, int64_t numOutputs,
                                     torch::DeviceType device) {
    net = NoResidualNN(numPlanes, width, height, numOutputs);
    net->to(device);
}

DefaultNoResidual::DefaultNoResidual(int64_t numPlanes, int64_t width, int64_t height, int64_t numOutputs,
                                     std::string fileName, torch::DeviceType device) {
    net = NoResidualNN(numPlanes, width, height, numOutputs);
    this->load(fileName);
    net->to(device);
}

DefaultNoResidual::~DefaultNoResidual() {
}

void DefaultNoResidual::load(std::string fileName) {
    torch::load(net, fileName);
}

void DefaultNoResidual::save(std::string fileName) {
    torch::save(net, fileName);
}

std::tuple<torch::Tensor, torch::Tensor> DefaultNoResidual::calculate(const torch::Tensor &input) {
    return net->forward(input);
}

void DefaultNoResidual::copyNetFrom(NeuralNetwork *copySource) {
    copySource->save("NeuralNets/buf");
    this->load("NeuralNets/buf");
}

void DefaultNoResidual::setLearningRate(const float &learningRate) {
    this->learningRate = learningRate;
}

void DefaultNoResidual::training(torch::Tensor &val, torch::Tensor &probs, torch::Tensor &probsTarget,
                                 torch::Tensor &valueTarget) {

    torch::optim::SGD optimizer(net->parameters(), learningRate);
    optimizer.zero_grad();

    auto lossValueV = torch::mse_loss(val, valueTarget);
    auto lossPolicy = -(probs*probsTarget);
    lossPolicy = lossPolicy.sum(1).mean();
    auto loss = lossPolicy + lossValueV;

    loss.backward();
    optimizer.step();
}

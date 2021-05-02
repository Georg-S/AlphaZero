#include "NeuralNetworks/DefaultNeuralNet.h"

DefaultNeuralNet::DefaultNeuralNet(int64_t numPlanes, int64_t width, int64_t height, int64_t numOutputs,
	torch::DeviceType device)
{
	net = CutDownAlphaGoZeroNet(numPlanes, width, height, numOutputs);
	net->to(device);
}

DefaultNeuralNet::DefaultNeuralNet(int64_t numPlanes, int64_t width, int64_t height, int64_t numOutputs,
	std::string fileName, torch::DeviceType device)
{
	net = CutDownAlphaGoZeroNet(numPlanes, width, height, numOutputs);
	this->load(fileName);
	net->to(device);
}

void DefaultNeuralNet::load(std::string fileName)
{
	torch::load(net, fileName);
}

void DefaultNeuralNet::save(std::string fileName)
{
	torch::save(net, fileName);
}

std::tuple<torch::Tensor, torch::Tensor> DefaultNeuralNet::calculate(const torch::Tensor& input)
{
	return net->forward(input);
}

void DefaultNeuralNet::copyNetFrom(NeuralNetwork* copySource)
{
	copySource->save("NeuralNets/buf");
	this->load("NeuralNets/buf");
}

void DefaultNeuralNet::setLearningRate(float learningRate)
{
	this->learningRate = learningRate;
}

void DefaultNeuralNet::training(torch::Tensor& val, torch::Tensor& probs, torch::Tensor& probsTarget,
	torch::Tensor& valueTarget)
{
	torch::optim::SGD optimizer(net->parameters(), learningRate);
	optimizer.zero_grad();

	auto lossValueV = torch::mse_loss(val, valueTarget);
	auto lossPolicy = -(probs * probsTarget);
	lossPolicy = lossPolicy.sum(1).mean();
	auto loss = lossPolicy + lossValueV;

	loss.backward();
	optimizer.step();
}

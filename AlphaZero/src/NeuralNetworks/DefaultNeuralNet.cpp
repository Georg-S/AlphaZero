#include "NeuralNetworks/DefaultNeuralNet.h"

DefaultNeuralNet::DefaultNeuralNet(int64_t numPlanes, int64_t width, int64_t height, int64_t numOutputs,
	torch::DeviceType device)
	: m_numPlanes(numPlanes)
	, m_width(width)
	, m_height(height)
	, m_numOutputs(numOutputs)
	, m_device(device)
{
	m_net = CutDownAlphaGoZeroNet(numPlanes, width, height, numOutputs);
	m_net->to(device);
	// By default if we create a new neural m_net, we are in training mode
	m_net->train();
}

DefaultNeuralNet::DefaultNeuralNet(int64_t numPlanes, int64_t width, int64_t height, int64_t numOutputs,
	std::string fileName, torch::DeviceType device)
	: m_numPlanes(numPlanes)
	, m_width(width)
	, m_height(height)
	, m_numOutputs(numOutputs)
	, m_device(device)
{
	m_net = CutDownAlphaGoZeroNet(numPlanes, width, height, numOutputs);
	load(fileName);
	m_net->to(device);
	// By default if we load a neural net, we are in eval mode (interference / production)
	m_net->eval();
}

DefaultNeuralNet::DefaultNeuralNet(CutDownAlphaGoZeroNet&& net, torch::DeviceType device)
	: m_net(std::move(net))
	, m_device(device)
{
};

void DefaultNeuralNet::load(std::string fileName)
{
	torch::load(m_net, fileName);
}

void DefaultNeuralNet::save(std::string fileName)
{
	torch::save(m_net, fileName);
}

std::tuple<torch::Tensor, torch::Tensor> DefaultNeuralNet::calculate(const torch::Tensor& input)
{
	return m_net->forward(input);
}

void DefaultNeuralNet::copyNetFrom(NeuralNetwork* copySource)
{
	copySource->save("NeuralNets/buf");
	this->load("NeuralNets/buf");
}

void DefaultNeuralNet::setLearningRate(float learningRate)
{
	this->m_learningRate = learningRate;
}

void DefaultNeuralNet::training(torch::Tensor& val, torch::Tensor& probs, torch::Tensor& probsTarget,
	torch::Tensor& valueTarget)
{
	torch::optim::SGD optimizer(m_net->parameters(), m_learningRate);
	optimizer.zero_grad();

	auto lossValueV = torch::mse_loss(val, valueTarget);
	auto lossPolicy = -(probs * probsTarget);
	lossPolicy = lossPolicy.sum(1).mean();
	auto loss = lossPolicy + lossValueV;

	loss.backward();
	optimizer.step();
}

void DefaultNeuralNet::setToEval()
{
	m_net->eval();
}

void DefaultNeuralNet::setToTraining()
{
	m_net->train();
}

std::unique_ptr<NeuralNetwork> DefaultNeuralNet::deepCopy() const
{
	// Tried to achieve a deepcopy by deriving from the nn::Cloneable class, but this didn't work out (weights seemed to be different)
	// Therefore we use a little workaround here to achieve a deep copy: Write the model to a stringstream and load it again
	std::ostringstream oss;
	torch::serialize::OutputArchive archive;
	m_net->save(archive);
	archive.save_to(oss);
	auto data = oss.str();

	std::istringstream iss(data);
	torch::serialize::InputArchive inpArchive;
	inpArchive.load_from(iss);

	CutDownAlphaGoZeroNet cloned = CutDownAlphaGoZeroNet(m_numPlanes, m_width, m_height, m_numOutputs);
	cloned->load(inpArchive);

	return std::make_unique<DefaultNeuralNet>(std::move(cloned), m_device);
}

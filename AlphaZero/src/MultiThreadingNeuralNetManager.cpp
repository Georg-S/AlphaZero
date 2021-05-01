#include "MultiThreadingNeuralNetManager.h"

MultiThreadingNeuralNetManager::MultiThreadingNeuralNetManager(int threadCount, NeuralNetwork* net) : threadCount(threadCount), net(net)
{
	elementsAdded = 0;
}

int MultiThreadingNeuralNetManager::addInputThreadSafe(torch::Tensor input)
{
	std::unique_lock<std::mutex> lock(this->threadingMutex);
	const int resultIndex = this->elementsAdded;

	if (this->inputBuffer.numel() == 0)
		this->inputBuffer = input;
	else
		this->inputBuffer = torch::cat({ this->inputBuffer, input}, 0);
	this->elementsAdded++;

	return resultIndex;
}

void MultiThreadingNeuralNetManager::calculateOutputThreadSafe()
{
	std::unique_lock<std::mutex> lock(this->threadingMutex);

	this->output = net->calculate(this->inputBuffer);
}

void MultiThreadingNeuralNetManager::clearInput()
{
	std::unique_lock<std::mutex> lock(this->threadingMutex);

	this->inputBuffer = torch::Tensor();
	this->elementsAdded = 0;
}


std::tuple<torch::Tensor, torch::Tensor> MultiThreadingNeuralNetManager::getOutput(int index)
{
	torch::Tensor valueTens = std::get<0>(this->output)[index];
	torch::Tensor probTens = std::get<1>(this->output)[index];

	return std::make_tuple(valueTens, probTens);
}

int MultiThreadingNeuralNetManager::getThreadCount() const
{
	return this->threadCount;
}

void MultiThreadingNeuralNetManager::waitUntilResultIsReady()
{
	std::unique_lock<std::mutex> lck(threadingMutex);
	cond.wait(lck);
}

void MultiThreadingNeuralNetManager::wakeUpAllThreads()
{
	std::unique_lock<std::mutex> lck(threadingMutex);
	cond.notify_all();
}


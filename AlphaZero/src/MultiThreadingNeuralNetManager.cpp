#include "MultiThreadingNeuralNetManager.h"

MultiThreadingNeuralNetManager::MultiThreadingNeuralNetManager()
{
	elementsAdded = 0;
}

void MultiThreadingNeuralNetManager::addInputThreadSafe(torch::Tensor input)
{
	std::unique_lock<std::mutex> lock(this->threadingMutex);

	if (this->inputBuffer.numel() == 0)
		this->inputBuffer = input;
	else
		this->inputBuffer = torch::cat({ this->inputBuffer, input}, 0);
	this->elementsAdded++;
}

void MultiThreadingNeuralNetManager::calculateOutputThreadSafe()
{
}

void MultiThreadingNeuralNetManager::clearInput()
{
	this->inputBuffer = torch::Tensor();
	this->elementsAdded = 0;
}


torch::Tensor MultiThreadingNeuralNetManager::getOutput()
{
	return torch::Tensor();
}


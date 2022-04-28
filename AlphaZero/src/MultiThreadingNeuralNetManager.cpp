#include "MultiThreadingNeuralNetManager.h"

MultiThreadingNeuralNetManager::MultiThreadingNeuralNetManager(int threadCount, int activeThreads, NeuralNetwork* net)
	: threadCount(threadCount)
	, activeThreads(activeThreads)
	, net(net)
	, elementsAdded(0)
{}

int MultiThreadingNeuralNetManager::addInputThreadSafe(torch::Tensor input)
{
	std::unique_lock<std::mutex> lock(this->threadingMutex);
	const int resultIndex = this->elementsAdded;

	if (this->inputBuffer.numel() == 0)
		this->inputBuffer = input;
	else
		this->inputBuffer = torch::cat({ this->inputBuffer, input }, 0);
	this->elementsAdded++;

	return resultIndex;
}

std::tuple<torch::Tensor, torch::Tensor> MultiThreadingNeuralNetManager::getOutput(int index)
{
	torch::Tensor valueTens = std::get<0>(this->output)[index];
	torch::Tensor probTens = std::get<1>(this->output)[index];

	return std::make_tuple(valueTens, probTens);
}

void MultiThreadingNeuralNetManager::safeDecrementActiveThreads()
{
	std::unique_lock<std::mutex> lck(threadingMutex);

	this->activeThreads--;
	if ((this->activeThreads != 0) && this->waitingThreads >= this->activeThreads)
		calculateAndWakeup();
}

void MultiThreadingNeuralNetManager::handleWaitingAndWakeup()
{
	std::unique_lock<std::mutex> lck(threadingMutex);
	if ((this->waitingThreads + 1) != this->activeThreads)
	{
		this->waitingThreads++;
		cond.wait(lck);
	}
	else
	{
		calculateAndWakeup();
	}
}

void MultiThreadingNeuralNetManager::calculateAndWakeup()
{
	calculateOutput();
	clearInput();
	wakeUpAllThreads();
}

int MultiThreadingNeuralNetManager::getThreadCount() const
{
	return this->threadCount;
}

void MultiThreadingNeuralNetManager::calculateOutput()
{
	this->output = net->calculate(this->inputBuffer);
}

void MultiThreadingNeuralNetManager::clearInput()
{
	this->inputBuffer = torch::Tensor();
	this->elementsAdded = 0;
}

void MultiThreadingNeuralNetManager::waitUntilResultIsReady()
{
	std::unique_lock<std::mutex> lck(threadingMutex);
	this->waitingThreads++;
	cond.wait(lck);
}

void MultiThreadingNeuralNetManager::wakeUpAllThreads()
{
	this->waitingThreads = 0;
	cond.notify_all();
}
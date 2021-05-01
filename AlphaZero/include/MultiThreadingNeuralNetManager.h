#ifndef DEEPREINFORCEMENTLEARNING_MULTITHREADINGNEURALNETMANAGER_H
#define DEEPREINFORCEMENTLEARNING_MULTITHREADINGNEURALNETMANAGER_H

#include <torch/torch.h>
#include <mutex>
#include <tuple>
#include "NeuralNetworks/NeuralNetwork.h"

class MultiThreadingNeuralNetManager 
{
public:
	MultiThreadingNeuralNetManager(int threadCount, NeuralNetwork* net);
	int addInputThreadSafe(torch::Tensor input);
	void calculateOutputThreadSafe();
	void clearInput();
	std::tuple<torch::Tensor, torch::Tensor> getOutput(int index);
	int getThreadCount() const;
	void waitUntilResultIsReady();
	void wakeUpAllThreads();

private:
	NeuralNetwork* net;
	torch::Tensor inputBuffer;
	std::tuple<torch::Tensor, torch::Tensor> output;
	std::condition_variable cond;
	std::mutex threadingMutex;
	int elementsAdded = 0;
	const int threadCount;
};

#endif //DEEPREINFORCEMENTLEARNING_MULTITHREADINGNEURALNETMANAGER_H
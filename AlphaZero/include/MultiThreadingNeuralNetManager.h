#ifndef DEEPREINFORCEMENTLEARNING_MULTITHREADINGNEURALNETMANAGER_H
#define DEEPREINFORCEMENTLEARNING_MULTITHREADINGNEURALNETMANAGER_H

#include <mutex>
#include <tuple>
// Libtorch has many warnings which clutter the output, so we ignore them
#pragma warning(push, 0)
#include <torch/torch.h>
#pragma warning(pop)
#include "NeuralNetworks/NeuralNetwork.h"

class MultiThreadingNeuralNetManager
{
public:
	MultiThreadingNeuralNetManager(int threadCount, int activeThreads, NeuralNetwork* net);
	int addInputThreadSafe(torch::Tensor input);
	std::tuple<torch::Tensor, torch::Tensor> getOutput(int index);
	void safeDecrementActiveThreads();
	void handleWaitingAndWakeup();
	void calculateAndWakeup();
	int getThreadCount() const;

private:
	void calculateOutput();
	void clearInput();
	void waitUntilResultIsReady();
	void wakeUpAllThreads();

	NeuralNetwork* net;
	torch::Tensor inputBuffer;
	std::tuple<torch::Tensor, torch::Tensor> output;
	std::condition_variable cond;
	std::mutex threadingMutex;
	int elementsAdded = 0;
	const int threadCount;
	int activeThreads = 0;
	int waitingThreads = 0;
};

#endif //DEEPREINFORCEMENTLEARNING_MULTITHREADINGNEURALNETMANAGER_H
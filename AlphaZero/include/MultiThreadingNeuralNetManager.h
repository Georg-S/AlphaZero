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
	void wakeUpAllThreads();

	NeuralNetwork* m_net;
	torch::Tensor m_inputBuffer;
	std::tuple<torch::Tensor, torch::Tensor> m_output;
	std::condition_variable m_cond;
	std::mutex m_threadingMutex;
	int m_elementsAdded = 0;
	const int m_threadCount;
	int m_activeThreads = 0;
	int m_waitingThreads = 0;
};

#endif //DEEPREINFORCEMENTLEARNING_MULTITHREADINGNEURALNETMANAGER_H
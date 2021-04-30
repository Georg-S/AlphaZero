#ifndef DEEPREINFORCEMENTLEARNING_MULTITHREADINGNEURALNETMANAGER_H
#define DEEPREINFORCEMENTLEARNING_MULTITHREADINGNEURALNETMANAGER_H

#include <torch/torch.h>
#include <mutex>

class MultiThreadingNeuralNetManager 
{
public:
	MultiThreadingNeuralNetManager();
	void addInputThreadSafe(torch::Tensor input);
	void calculateOutputThreadSafe();
	void clearInput();
	torch::Tensor getOutput();

private:
	torch::Tensor inputBuffer;
	torch::Tensor output;
	std::mutex threadingMutex;
	int elementsAdded = 0;
};

#endif //DEEPREINFORCEMENTLEARNING_MULTITHREADINGNEURALNETMANAGER_H
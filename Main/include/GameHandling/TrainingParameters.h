#ifndef DEEPREINFORCEMENTLEARNING_TRAININGPARAMETERS_H
#define DEEPREINFORCEMENTLEARNING_TRAININGPARAMETERS_H

#include <torch/torch.h>

struct TrainingParameters {
	bool useDraws;
	bool restrictGameLength;
	float learningRate;
	int maxGameLength;
	int replayMemorySize;
	int trainingIterations;
	int selfPlayMctsCount;
	int selfPlayGamesCount;
	int trainingBatchSize;
	int saveIterationCount;
	int randomizedMoveCount;
	int filterCount;
	int cpuThreads;
	torch::DeviceType device;
};

#endif //DEEPREINFORCEMENTLEARNING_TRAININGPARAMETERS_H

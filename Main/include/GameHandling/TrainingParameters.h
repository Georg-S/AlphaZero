#ifndef DEEPREINFORCEMENTLEARNING_TRAININGPARAMETERS_H
#define DEEPREINFORCEMENTLEARNING_TRAININGPARAMETERS_H

// Libtorch has many warnings which clutter the output, so we ignore them
#pragma warning(push, 0)
#include <torch/torch.h>
#pragma warning(pop)

struct TrainingParameters
{
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

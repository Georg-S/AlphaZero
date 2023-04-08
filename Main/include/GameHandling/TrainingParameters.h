#ifndef DEEPREINFORCEMENTLEARNING_TRAININGPARAMETERS_H
#define DEEPREINFORCEMENTLEARNING_TRAININGPARAMETERS_H

// Libtorch has many warnings which clutter the output, so we ignore them
#pragma warning(push, 0)
#include <torch/torch.h>
#include <AlphaZeroTraining.h>
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

	AlphaZeroTraining::Parameters getAlphaZeroParams(std::string neuralNetPath, const AlphaZeroTraining::Parameters& param = {}) const
	{
		auto parameters = param;
		parameters.neuralNetPath = std::move(neuralNetPath);
		parameters.MAX_REPLAY_MEMORY_SIZE = replayMemorySize;
		parameters.TRAINING_DONT_USE_DRAWS = !useDraws;
		parameters.RESTRICT_GAME_LENGTH = restrictGameLength;
		parameters.DRAW_AFTER_COUNT_OF_STEPS = maxGameLength;
		parameters.TRAINING_ITERATIONS = trainingIterations;
		parameters.SELF_PLAY_MCTS_COUNT = selfPlayMctsCount;
		parameters.NUM_SELF_PLAY_GAMES = selfPlayGamesCount;
		parameters.TRAINING_BATCH_SIZE = trainingBatchSize;
		parameters.SAVE_ITERATION_COUNT = saveIterationCount;
		parameters.RANDOM_MOVE_COUNT = randomizedMoveCount;
		parameters.NUMBER_CPU_THREADS = cpuThreads;

		return parameters;
	}
};

#endif //DEEPREINFORCEMENTLEARNING_TRAININGPARAMETERS_H

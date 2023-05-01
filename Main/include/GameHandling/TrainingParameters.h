#ifndef DEEPREINFORCEMENTLEARNING_TRAININGPARAMETERS_H
#define DEEPREINFORCEMENTLEARNING_TRAININGPARAMETERS_H

// Libtorch has many warnings which clutter the output, so we ignore them
#pragma warning(push, 0)
#include <torch/torch.h>
#pragma warning(pop)
#include <AlphaZeroTrainingTemplate.h>

struct TrainingParameters
{
	bool useDraws;
	bool restrictGameLength;
	float learningRate;
	int maxGameLength;
	int replayMemorySize;
	int trainingIterations;
	int selfPlayMctsCount;
	int selfPlayBatchSize;
	int selfPlayGamesCount;
	int trainingBatchSize;
	int saveIterationCount;
	int randomizedMoveCount;
	int filterCount;
	int cpuThreads;
	torch::DeviceType device;

	AlphaZeroTrainingParameters getAlphaZeroParams(std::string neuralNetPath, AlphaZeroTrainingParameters param = {}) const
	{
		param.neuralNetPath = std::move(neuralNetPath);
		param.MAX_REPLAY_MEMORY_SIZE = replayMemorySize;
		param.TRAINING_DONT_USE_DRAWS = !useDraws;
		param.RESTRICT_GAME_LENGTH = restrictGameLength;
		param.DRAW_AFTER_COUNT_OF_STEPS = maxGameLength;
		param.TRAINING_ITERATIONS = trainingIterations;
		param.SELF_PLAY_MCTS_COUNT = selfPlayMctsCount;
		param.SELFPLAY_BATCH_SIZE = selfPlayBatchSize;
		param.NUM_SELF_PLAY_GAMES = selfPlayGamesCount;
		param.TRAINING_BATCH_SIZE = trainingBatchSize;
		param.SAVE_ITERATION_COUNT = saveIterationCount;
		param.RANDOM_MOVE_COUNT = randomizedMoveCount;
		param.NUMBER_CPU_THREADS = cpuThreads;

		return param;
	}
};

#endif //DEEPREINFORCEMENTLEARNING_TRAININGPARAMETERS_H

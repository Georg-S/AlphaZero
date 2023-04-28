#ifndef DEEPREINFORCEMENTLEARNING_ALPHAZEROTRAINING_H
#define DEEPREINFORCEMENTLEARNING_ALPHAZEROTRAINING_H

#include <string>
#include <vector>
#include <time.h>
#include <cfloat>
#include <thread>
#include <mutex>
#include "AlphaZeroUtility.h"
#include "NeuralNetworks/NeuralNetwork.h"
#include "Game.h"
#include "RingBuffer.h"
#include "MonteCarloTreeSearch.h"
#include "ReplayElement.h"

class AlphaZeroTraining
{
public:
	struct Parameters
	{
		std::string neuralNetPath;

		//Training Parameters
		bool TRAINING_DONT_USE_DRAWS = false;
		int CURRENT_ITERATION = 0;
		int SELFPLAY_BATCH_SIZE = 10;
		int TRAINING_ITERATIONS = 10000;
		int MIN_REPLAY_MEMORY_SIZE = 100;
		int SELF_PLAY_MCTS_COUNT = 100;
		int NUM_SELF_PLAY_GAMES = 100;
		int TRAINING_BATCH_SIZE = 100;
		int SAVE_ITERATION_COUNT = 10;
		int RANDOM_MOVE_COUNT = 15;
		int NUMBER_CPU_THREADS = 1;
		int MAX_REPLAY_MEMORY_SIZE = 40000;

		bool RESTRICT_GAME_LENGTH = true;
		int DRAW_AFTER_COUNT_OF_STEPS = 75;
	};

	AlphaZeroTraining(int actionCount, NeuralNetwork* currentBest, torch::DeviceType = torch::kCPU);
	void runTraining(Game* game);
	// Sets the training parameters and creates a new replay buffer -> all elements in replay buffer get destroyed
	void setTrainingParams(Parameters params);

private:
	void selfPlay(NeuralNetwork* net, Game* game);
	void selfPlayBuf(NeuralNetwork* net, Game* game);
	std::vector<ReplayElement> selfPlay(NeuralNetwork* net, Game* game, int batchSize);
	void printMemoryUsage(long long cacheMemSize, long long mctsMemsize, long long trainingsDataTotalSize) const;
	void addResult(std::vector<ReplayElement>& elements, int winner);
	void trainNet(NeuralNetwork* net, Game* game);
	void trainNetMultiThreaded(NeuralNetwork* net, Game* game);
	torch::Tensor convertSampleToNeuralInput(const std::vector<ReplayElement>& sample, Game* game);
	torch::Tensor convertToValueTarget(const std::vector<ReplayElement>& sample);
	torch::Tensor convertToProbsTarget(const std::vector<ReplayElement>& sample);
	void save(int iteration);

	RingBuffer<ReplayElement> m_replayMemory;
	NeuralNetwork* m_neuralNet;
	int m_actionCount = -1;
	torch::DeviceType m_device;
	int m_gamesToPlay = 0;
	size_t m_trainingBatchIndex = 0;
	std::mutex m_mut;
	Parameters m_params;
};

#endif //DEEPREINFORCEMENTLEARNING_ALPHAZEROTRAINING_H

#ifndef DEEPREINFORCEMENTLEARNING_ALPHAZEROTRAINING_H
#define DEEPREINFORCEMENTLEARNING_ALPHAZEROTRAINING_H

#include <string>
#include <vector>
#include <time.h>
#include <cfloat>
#include <thread>
#include <mutex>
#include "MultiThreadingNeuralNetManager.h"
#include "NeuralNetworks/NeuralNetwork.h"
#include "Game.h"
#include "RingBuffer.h"
#include "MonteCarloTreeSearch.h"
#include "ReplayElement.h"

class AlphaZeroTraining 
{
public:
	AlphaZeroTraining(int actionCount, NeuralNetwork* currentBest, torch::DeviceType = torch::kCPU);
	void runTraining(Game* game);
	void selfPlay(NeuralNetwork* net, Game* game);
	void selfPlaySingleThread(NeuralNetwork* net, Game* game);
	void selfPlayMultiThread(NeuralNetwork* net, Game* game);
	void selfPlayMultiThreadGames(NeuralNetwork* net, Game* game, int& gamesToPlay, MultiThreadingNeuralNetManager* threadManager);
	std::vector<ReplayElement> selfPlayGame(NeuralNetwork* net, Game* game, bool multiThreading);
	static int getRandomAction(const std::vector<float>& probabilities);
	void addResult(std::vector<ReplayElement>& elements, int winner);
	void trainNet(NeuralNetwork* net, Game* game);
	torch::Tensor convertSampleToNeuralInput(const std::vector<ReplayElement>& sample, Game* game);
	torch::Tensor convertToValueTarget(const std::vector<ReplayElement>& sample);
	torch::Tensor convertToProbsTarget(const std::vector<ReplayElement>& sample);
	int getArgMaxIndex(const std::vector<float>& vec);
	void save(int iteration);
	void setMaxReplayMemorySize(int size);

private:
	RingBuffer<ReplayElement> replayMemory;
	std::unique_ptr<MultiThreadingNeuralNetManager> threadManager;
	int actionCount = -1;
	torch::DeviceType device;
	NeuralNetwork* neuralNet;
	std::mutex mut;


	int MAX_REPLAY_MEMORY_SIZE = 40000;

public:
	std::string neuralNetPath;


	//Training Flags
	bool TRAINING_DONT_USE_DRAWS = false;
	//Training Parameters
	int TRAINING_ITERATIONS = 10000;
	int MIN_REPLAY_MEMORY_SIZE = 100;
	int SELF_PLAY_MCTS_COUNT = 100;
	int NUM_SELF_PLAY_GAMES = 100;
	int TRAINING_BATCH_SIZE = 100;
	int SAVE_ITERATION_COUNT = 10;
	int RANDOM_MOVE_COUNT = 15;
	int NUMBER_CPU_THREADS = 1;

	bool RESTRICT_GAME_LENGTH = true;
	int DRAW_AFTER_COUNT_OF_STEPS = 75;
};


#endif //DEEPREINFORCEMENTLEARNING_ALPHAZEROTRAINING_H

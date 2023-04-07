#include <gtest/gtest.h>
#include <iostream>
#include <TicTacToe/TicTacToeAdapter.h>
#include <AlphaZeroTraining.h>
#include <NeuralNetworks/DefaultNeuralNet.h>
#include "TestConfig.h"

#if RunTests

TEST(Training, test_random_action)
{
	srand(time(NULL));
	const int amount = 1000000;
	std::vector<float> test{ 0.f, 0.2f, 0.1f, 0.5, 0.1, 0.1, 0.0, 0.0, 0.0 };
	std::vector<int> results = std::vector<int>(9, 0);

	for (int i = 0; i < amount; i++)
	{
		int buf = ALZ::getRandomIndex(test, 1.0);
		results[buf] += 1;
	}

	EXPECT_NEAR(results[3], amount / 2, amount * 0.01);
}

TEST(Training, test_mult_thread_training)
{
	srand(time(NULL));
	torch::DeviceType device = torch::kCUDA;
	TicTacToeAdapter adap = TicTacToeAdapter();

	DefaultNeuralNet neuralNet = DefaultNeuralNet(2, 3, 3, 9, device);
	AlphaZeroTraining training = AlphaZeroTraining(9, &neuralNet, device);

	auto trainingParams = AlphaZeroTraining::Parameters{};
	trainingParams.TRAINING_DONT_USE_DRAWS = false;
	trainingParams.RESTRICT_GAME_LENGTH = false;
	trainingParams.DRAW_AFTER_COUNT_OF_STEPS = 50;
	trainingParams.TRAINING_ITERATIONS = 100;
	trainingParams.MAX_REPLAY_MEMORY_SIZE = 40000;
	trainingParams.MIN_REPLAY_MEMORY_SIZE = 100;
	trainingParams.SELF_PLAY_MCTS_COUNT = 50;
	trainingParams.NUM_SELF_PLAY_GAMES = 10;
	trainingParams.TRAINING_BATCH_SIZE = 100;
	trainingParams.SAVE_ITERATION_COUNT = 1;
	trainingParams.RANDOM_MOVE_COUNT = 3;
	trainingParams.NUMBER_CPU_THREADS = 10;
	training.setTrainingParams(trainingParams);

	training.selfPlayMultiThread(&neuralNet, &adap);
}

#endif //RunTests

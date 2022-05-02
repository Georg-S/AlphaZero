#include <gtest/gtest.h>
#include <iostream>
#include <TicTacToe/TicTacToeAdapter.h>
#include <AlphaZeroTraining.h>
#include <NeuralNetworks/DefaultNeuralNet.h>

#if 1

TEST(Training, test_random_action)
{
	srand(time(NULL));
	const int amount = 1000000;
	std::vector<float> test{ 0.f, 0.2f, 0.1f, 0.5, 0.1, 0.1, 0.0, 0.0, 0.0 };
	std::vector<int> results = std::vector<int>(9, 0);

	for (int i = 0; i < amount; i++)
	{
		int buf = AlphaZeroTraining::getRandomAction(test);
		results[buf] += 1;
	}

	EXPECT_NEAR(results[3], amount / 2, amount * 0.01);
}

TEST(Training, test_mult_thread_training)
{
	srand(time(NULL));
	torch::DeviceType device = torch::kCUDA;
	TicTacToeAdapter adap = TicTacToeAdapter();

	DefaultNeuralNet* neuralNet = new DefaultNeuralNet(2, 3, 3, 9, device);
	AlphaZeroTraining training = AlphaZeroTraining(9, neuralNet, device);

	training.TRAINING_DONT_USE_DRAWS = false;
	training.RESTRICT_GAME_LENGTH = false;
	training.DRAW_AFTER_COUNT_OF_STEPS = 50;
	training.TRAINING_ITERATIONS = 100;
	training.setMaxReplayMemorySize(40000);
	training.MIN_REPLAY_MEMORY_SIZE = 100;
	training.SELF_PLAY_MCTS_COUNT = 50;
	training.NUM_SELF_PLAY_GAMES = 10;
	training.TRAINING_BATCH_SIZE = 100;
	training.SAVE_ITERATION_COUNT = 1;
	training.RANDOM_MOVE_COUNT = 3;
	training.NUMBER_CPU_THREADS = 10;

	training.selfPlay(neuralNet, &adap);
}

#endif
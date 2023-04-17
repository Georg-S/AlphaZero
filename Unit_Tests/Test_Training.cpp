#include <gtest/gtest.h>
#include <iostream>
#include <TicTacToe/TicTacToeAdapter.h>
#include <AlphaZeroTraining.h>
#include <Chess/ChessAdapter.h>
#include <NeuralNetworks/DefaultNeuralNet.h>
#include "TestConfig.h"

//TEST(Training, GameBatchTraining) 
//{
//	torch::DeviceType device = torch::kCUDA;
//
//	//DefaultNeuralNet neuralNet = DefaultNeuralNet(14, 8, 8, 4096, device);
//	//ChessAdapter adap = ChessAdapter();
//	//AlphaZeroTraining training = AlphaZeroTraining(adap.getActionCount(), &neuralNet, device);
//
//	TicTacToeAdapter adap = TicTacToeAdapter();
//	DefaultNeuralNet neuralNet = DefaultNeuralNet(2, 3, 3, 9, device);
//	AlphaZeroTraining training = AlphaZeroTraining(adap.getActionCount(), &neuralNet, device);
//
//	auto trainingParams = AlphaZeroTraining::Parameters{};
//	trainingParams.TRAINING_DONT_USE_DRAWS = true;
//	trainingParams.RESTRICT_GAME_LENGTH = true;
//	trainingParams.DRAW_AFTER_COUNT_OF_STEPS = 80;
//	trainingParams.TRAINING_ITERATIONS = 1;
//	trainingParams.MAX_REPLAY_MEMORY_SIZE = 300000;
//	trainingParams.MIN_REPLAY_MEMORY_SIZE = 100;
//	trainingParams.SELF_PLAY_MCTS_COUNT = 50;
//	trainingParams.NUM_SELF_PLAY_GAMES = 1000;
//	trainingParams.TRAINING_BATCH_SIZE = 100;
//	trainingParams.SAVE_ITERATION_COUNT = 1;
//	trainingParams.RANDOM_MOVE_COUNT = 3;
//	trainingParams.NUMBER_CPU_THREADS = 10;
//	trainingParams.SELFPLAY_BATCH_SIZE = 1000;
//	training.setTrainingParams(trainingParams);
//
//}

#if RunTests

TEST(Training, test_random_action)
{
	srand(time(NULL));
	const int amount = 1000000;
	std::vector<std::pair<int, float>> test{ {0, 0.f}, {1, 0.2f}, {2, 0.1f}, {3, 0.5}, {4, 0.1}, {5, 0.1}, {6, 0.0}, {7, 0.0}, {8, 0.0} };
	std::vector<int> results = std::vector<int>(9, 0);

	for (int i = 0; i < amount; i++)
	{
		int buf = ALZ::getRandomAction(test);
		results[buf] += 1;
	}

	EXPECT_NEAR(results[3], amount / 2, amount * 0.01);
}

TEST(Training, test_mult_thread_training)
{
	torch::DeviceType device = torch::kCUDA;
	TicTacToeAdapter adap = TicTacToeAdapter();

	DefaultNeuralNet neuralNet = DefaultNeuralNet(2, 3, 3, 9, device);
	AlphaZeroTraining training = AlphaZeroTraining(9, &neuralNet, device);

	auto trainingParams = AlphaZeroTraining::Parameters{};
	trainingParams.TRAINING_DONT_USE_DRAWS = false;
	trainingParams.RESTRICT_GAME_LENGTH = false;
	trainingParams.DRAW_AFTER_COUNT_OF_STEPS = 50;
	trainingParams.TRAINING_ITERATIONS = 10;
	trainingParams.MAX_REPLAY_MEMORY_SIZE = 40000;
	trainingParams.MIN_REPLAY_MEMORY_SIZE = 100;
	trainingParams.SELF_PLAY_MCTS_COUNT = 50;
	trainingParams.NUM_SELF_PLAY_GAMES = 10;
	trainingParams.SELFPLAY_BATCH_SIZE = 1;
	trainingParams.TRAINING_BATCH_SIZE = 100;
	trainingParams.SAVE_ITERATION_COUNT = 1;
	trainingParams.RANDOM_MOVE_COUNT = 3;
	trainingParams.NUMBER_CPU_THREADS = 10;
	training.setTrainingParams(trainingParams);

	//training.runTraining(&adap);
}

#endif //RunTests

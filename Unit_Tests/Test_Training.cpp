#include <gtest/gtest.h>
#include <iostream>
#include <TicTacToe/TicTacToeAdapter.h>
#include <Chess/ChessAdapter.h>
#include <NeuralNetworks/DefaultNeuralNet.h>
#include <AlphaZeroTrainingTemplate.h>

#include "TestConfig.h"
#include "Test_Utility.h"

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

static torch::DeviceType device = torch::kCUDA;
static TicTacToeAdapter tttAdap = TicTacToeAdapter();
static DefaultNeuralNet tttNet = DefaultNeuralNet(2, 3, 3, 9, device);

#if RunTrainingTest

TEST(Training, test_tic_tac_toe_training)
{
	auto trainingParams = AlphaZeroTrainingParameters{};
	trainingParams.neuralNetPath = "NeuralNets/Training/TicTacToe";
	trainingParams.TRAINING_DONT_USE_DRAWS = false;
	trainingParams.RESTRICT_GAME_LENGTH = false;
	trainingParams.DRAW_AFTER_COUNT_OF_STEPS = 50;
	trainingParams.TRAINING_ITERATIONS = 20;
	trainingParams.MAX_REPLAY_MEMORY_SIZE = 100000;
	trainingParams.MIN_REPLAY_MEMORY_SIZE = 100;
	trainingParams.SELF_PLAY_MCTS_COUNT = 50;
	trainingParams.NUM_SELF_PLAY_GAMES = 1000;
	trainingParams.SELFPLAY_BATCH_SIZE = 500;
	trainingParams.TRAINING_BATCH_SIZE = 1000;
	trainingParams.SAVE_ITERATION_COUNT = 1;
	trainingParams.RANDOM_MOVE_COUNT = 3;
	trainingParams.NUMBER_CPU_THREADS = 2;

	auto training = AlphaZeroTrainingT<ttt::Board, TicTacToeAdapter, false>(&tttAdap, &tttNet, device);
	training.setTrainingParams(trainingParams);
	training.runTraining();
}

#endif //RunTrainingTest

#endif //RunTests

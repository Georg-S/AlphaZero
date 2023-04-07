#include <gtest/gtest.h>
#include <MonteCarloTreeSearch.h>
#include <NeuralNetworks/DefaultNeuralNet.h>
#include <TicTacToe/TicTacToeAdapter.h>
#include <tuple>
#include <thread>
#include <vector>
#include <MultiThreadingNeuralNetManager.h>
#include "TestConfig.h"

#if RunTests

TEST(MonteCarloTreeSearch, test_sum_vector)
{
	std::vector<int> test{ 0,1,2,3,4,5,6,7,8,9 };
	int sum = ALZ::sum(test);

	ASSERT_EQ(sum, 45);
}

TEST(MonteCarloTreeSearch, test_sum_vector_0_size_vector)
{
	std::vector<int> test;
	int sum = ALZ::sum(test);

	ASSERT_FLOAT_EQ(sum, 0);
}

TEST(MonteCarloTreeSearch, test_mcts_ttt_draw_board)
{
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(9);
	DefaultNeuralNet net(2, 3, 3, 9);
	TicTacToeAdapter adap = TicTacToeAdapter();
	int result = mcts.search("OXXXXOOOX", &net, &adap, 1);

	ASSERT_EQ(result, 0);
}


TEST(MonteCarloTreeSearch, test_mcts_ttt_player_one_wins)
{
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(9);
	DefaultNeuralNet net(2, 3, 3, 9);
	TicTacToeAdapter adap = TicTacToeAdapter();
	float result = mcts.search("XOXOXOOXX", &net, &adap, 1);

	ASSERT_FLOAT_EQ(result, -1);
}

TEST(MonteCarloTreeSearch, test_mcts_ttt_player_two_wins)
{
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(9);
	DefaultNeuralNet net(2, 3, 3, 9);
	TicTacToeAdapter adap = TicTacToeAdapter();
	float result = mcts.search("XXOOOXOXX", &net, &adap, 2);

	ASSERT_FLOAT_EQ(result, -1);
}

TEST(MonteCarloTreeSearch, test_ttt_get_probabilities_one_move_possible)
{
	std::string state = "XXOO-XOXX";
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(9);
	DefaultNeuralNet net(2, 3, 3, 9);
	TicTacToeAdapter adap = TicTacToeAdapter();
	mcts.search(4, state, &net, &adap, 2);
	std::vector<float> probs = mcts.getProbabilities(state);

	ASSERT_FLOAT_EQ(probs[0], 0);
	ASSERT_FLOAT_EQ(probs[1], 0);
	ASSERT_FLOAT_EQ(probs[2], 0);
	ASSERT_FLOAT_EQ(probs[3], 0);
	ASSERT_FLOAT_EQ(probs[4], 1);
	ASSERT_FLOAT_EQ(probs[5], 0);
	ASSERT_FLOAT_EQ(probs[6], 0);
	ASSERT_FLOAT_EQ(probs[7], 0);
	ASSERT_FLOAT_EQ(probs[8], 0);
}

TEST(MonteCarloTreeSearch, test_ttt_get_probabilities_two_moves_possible_one_wins)
{
	std::string state = "OXOXOXX--";
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(9);
	DefaultNeuralNet net(2, 3, 3, 9);
	TicTacToeAdapter adap = TicTacToeAdapter();
	mcts.search(100, state, &net, &adap, 2);
	std::vector<float> probs = mcts.getProbabilities(state);

	ASSERT_GT(probs[8], probs[7]);
}


TEST(MonteCarloTreeSearch, test_ttt_get_probabilities_two_moves_possible_one_gets_draw)
{
	std::string state = "OXOXXO--X";
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(9);
	DefaultNeuralNet net(2, 3, 3, 9);
	TicTacToeAdapter adap = TicTacToeAdapter();
	mcts.search(100, state, &net, &adap, 2);
	std::vector<float> probs = mcts.getProbabilities(state);

	ASSERT_GT(probs[7], probs[6]);
}

void test_mcts(MultiThreadingNeuralNetManager& manager, NeuralNetwork* net)
{
	constexpr int actionCount = 9;
	TicTacToeAdapter game = TicTacToeAdapter();
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(actionCount);
	std::string currentState = "X-OOX-X-O";

	mcts.multiThreadedSearch(100, currentState, &game, 1, &manager);
	std::vector<float> probs = mcts.getProbabilities(currentState);
	manager.safeDecrementActiveThreads();

	ASSERT_GT(probs[5], probs[1]);
	ASSERT_GT(probs[5], probs[7]);
}

TEST(MonteCarloTreeSearch, test_ttt_multi_threading_mcts)
{
	constexpr int threadCount = 2;
	constexpr int actionCount = 9;
	std::vector<std::thread> threadPool;
	DefaultNeuralNet net(2, 3, 3, actionCount);
	MultiThreadingNeuralNetManager manager(threadCount, threadCount, &net);

	for (int i = 0; i < threadCount; i++)
		threadPool.push_back(std::thread(test_mcts, std::ref(manager), &net));

	for (auto& thread : threadPool)
		thread.join();
}

#endif //RunTests
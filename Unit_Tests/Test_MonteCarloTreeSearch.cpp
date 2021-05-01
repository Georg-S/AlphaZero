#include <gtest/gtest.h>
#include <MonteCarloTreeSearch.h>
#include <NeuralNetworks/DefaultNeuralNet.h>
#include <TicTacToe/TicTacToeAdapter.h>
#include <tuple>
#include <thread>
#include <vector>
#include <MultiThreadingNeuralNetManager.h>


TEST(MonteCarloTreeSearch, test_sum_vector)
{
	std::vector<int> test{ 0,1,2,3,4,5,6,7,8,9 };
	int sum = MonteCarloTreeSearch::sum(test);

	ASSERT_EQ(sum, 45);
}

TEST(MonteCarloTreeSearch, test_sum_vector_0_size_vector)
{
	std::vector<int> test;
	int sum = MonteCarloTreeSearch::sum(test);

	ASSERT_FLOAT_EQ(sum, 0);
}

TEST(MonteCarloTreeSearch, test_mcts_ttt_draw_board)
{
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(9);
	DefaultNeuralNet net(2, 3, 3, 9);
	TicTacToeAdapter adap = TicTacToeAdapter();
	int result = mcts.search("211112221", &net, &adap, 1);

	ASSERT_EQ(result, 0);
}


TEST(MonteCarloTreeSearch, test_mcts_ttt_player_one_wins)
{
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(9);
	DefaultNeuralNet net(2, 3, 3, 9);
	TicTacToeAdapter adap = TicTacToeAdapter();
	float result = mcts.search("121212211", &net, &adap, 1);

	ASSERT_FLOAT_EQ(result, -1);
}

TEST(MonteCarloTreeSearch, test_mcts_ttt_player_two_wins)
{
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(9);
	DefaultNeuralNet net(2, 3, 3, 9);
	TicTacToeAdapter adap = TicTacToeAdapter();
	float result = mcts.search("112221211", &net, &adap, 2);

	ASSERT_FLOAT_EQ(result, -1);
}

TEST(MonteCarloTreeSearch, test_ttt_get_probabilities_one_move_possible)
{
	std::string state = "112201211";
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
	std::string state = "212121100";
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(9);
	DefaultNeuralNet net(2, 3, 3, 9);
	TicTacToeAdapter adap = TicTacToeAdapter();
	mcts.search(100, state, &net, &adap, 2);
	std::vector<float> probs = mcts.getProbabilities(state);

	ASSERT_GT(probs[8], probs[7]);
}


TEST(MonteCarloTreeSearch, test_ttt_get_probabilities_two_moves_possible_one_gets_draw)
{
	std::string state = "212112001";
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
	TicTacToeAdapter adap = TicTacToeAdapter();
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(actionCount);
	std::string state = adap.getInitialGameState();
	mcts.multiThreadedSearch(state, net, &adap, 1, &manager);
}

TEST(MonteCarloTreeSearch, test_ttt_multi_threading_mcts) 
{
	constexpr int threadCount = 10;
	constexpr int actionCount = 9;
	std::vector<std::thread> threadPool;
	DefaultNeuralNet net(2, 3, 3, actionCount);
	MultiThreadingNeuralNetManager manager(threadCount, &net);

	for (int i = 0; i < threadCount; i++) 
		threadPool.push_back(std::thread(test_mcts, std::ref(manager), &net));

	for (auto& thread : threadPool)
		thread.join();
}

/*
TEST(MonteCarloTreeSearch, test_MultiThreadinNeuralNetManager)
{
	constexpr int threads = 2;
	constexpr int actionCount = 9;
	std::vector<std::thread> threadPool;
	TicTacToeAdapter adap = TicTacToeAdapter();
	DefaultNeuralNet net(2, 3, 3, actionCount);
	MultiThreadingNeuralNetManager manager(threads, &net);
	std::string state = adap.getInitialGameState();

	auto inp1 = adap.convertStateToNeuralNetInput(state, 1);
	auto inp2 = adap.convertStateToNeuralNetInput(state, 1);


	manager.addInputThreadSafe(inp1);
	manager.addInputThreadSafe(inp2);

	manager.calculateOutputThreadSafe();

	auto result = manager.getOutput(1);
	auto convert = std::get<0>(result).detach().to(torch::kCPU);
	std::cout << convert << std::endl;
}
*/
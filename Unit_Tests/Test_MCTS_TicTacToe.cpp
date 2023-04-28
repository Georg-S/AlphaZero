#include <gtest/gtest.h>
#include <MonteCarloTreeSearch.h>
#include <NeuralNetworks/DefaultNeuralNet.h>
#include <TicTacToe/TicTacToeAdapter.h>
#include <tuple>
#include <thread>
#include <vector>
#include <MonteCarloTreeSearchTemplate.h>

#include "TestConfig.h"
#include "Test_Utility.h"

#if RunTests

static torch::DeviceType device = torch::kCPU;
static TicTacToeAdapter tttAdap = TicTacToeAdapter();
static DefaultNeuralNet net(2, 3, 3, 9);

TEST(MCTS_TicTacToe, test_mcts_ttt_draw_board)
{
	const std::string boardStr = "OXXXXOOOX";
	auto board = ttt::Board(boardStr);
	auto mctsCache = MonteCarloTreeSearchCacheT<ttt::Board, TicTacToeAdapter, true>(torch::kCPU, &tttAdap);
	auto mcts = MonteCarloTreeSearchT<ttt::Board, TicTacToeAdapter, true>(&mctsCache, &tttAdap, device);
	float result = mcts.search(board, &net, 1);

	ASSERT_FLOAT_EQ(result, 0);
}

TEST(MCTS_TicTacToe, test_mcts_ttt_player_one_wins)
{
	const std::string boardStr = "XOXOXOOXX";
	auto mctsCache = MonteCarloTreeSearchCacheT<ttt::Board, TicTacToeAdapter, true>(torch::kCPU, &tttAdap);
	auto mcts = MonteCarloTreeSearchT<ttt::Board, TicTacToeAdapter, true>(&mctsCache, &tttAdap, device);
	auto board = ttt::Board(boardStr);
	float result = mcts.search(board, &net, 1);

	ASSERT_FLOAT_EQ(result, -1);
}

TEST(MCTS_TicTacToe, test_mcts_ttt_player_two_wins)
{
	const std::string boardStr = "XXOOOXOXX";
	auto board = ttt::Board(boardStr);
	auto mctsCache = MonteCarloTreeSearchCacheT<ttt::Board, TicTacToeAdapter, true>(torch::kCPU, &tttAdap);
	auto mcts = MonteCarloTreeSearchT<ttt::Board, TicTacToeAdapter, true>(&mctsCache, &tttAdap, device);
	float result = mcts.search(board, &net, 2);

	ASSERT_FLOAT_EQ(result, -1);
}

TEST(MCTS_TicTacToe, test_ttt_get_probabilities_one_move_possible)
{
	const std::string boardStr = "XXOO-XOXX";
	auto board = ttt::Board(boardStr);
	auto mctsCache = MonteCarloTreeSearchCacheT<ttt::Board, TicTacToeAdapter, true>(torch::kCPU, &tttAdap);
	auto mcts = MonteCarloTreeSearchT<ttt::Board, TicTacToeAdapter, true>(&mctsCache, &tttAdap, device);
	auto moves = tttAdap.getAllPossibleMoves(board, 2);
	mcts.search(4, board, &net, 2);

	std::vector<float> probs = getAllActionProbabilities(mcts.getProbabilities(board), tttAdap.getActionCount());

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

TEST(MCTS_TicTacToe, test_ttt_get_probabilities_two_moves_possible_one_wins_mock_expansion)
{
	const std::string boardStr = "OXOXOXX--";
	auto board = ttt::Board(boardStr);
	auto mctsCache = MonteCarloTreeSearchCacheT<ttt::Board, TicTacToeAdapter, true>(torch::kCPU, &tttAdap);
	auto mcts = MonteCarloTreeSearchT<ttt::Board, TicTacToeAdapter, true>(&mctsCache, &tttAdap, device);
	mcts.search(100, board, &net, 2);

	std::vector<float> probs = getAllActionProbabilities(mcts.getProbabilities(board), tttAdap.getActionCount());

	ASSERT_GT(probs[8], probs[7]);
}

TEST(MCTS_TicTacToe, test_ttt_get_probabilities_two_moves_possible_one_wins_real_expansion)
{
	const std::string boardStr = "OXOXOXX--";
	auto board = ttt::Board(boardStr);
	auto mctsCache = MonteCarloTreeSearchCacheT<ttt::Board, TicTacToeAdapter, false>(torch::kCPU, &tttAdap);
	auto mcts = MonteCarloTreeSearchT<ttt::Board, TicTacToeAdapter, false>(&mctsCache, &tttAdap, device);
	mcts.search(100, board, &net, 2);

	std::vector<float> probs = getAllActionProbabilities(mcts.getProbabilities(board), tttAdap.getActionCount());

	ASSERT_GT(probs[8], probs[7]);
}

TEST(MCTS_TicTacToe, test_ttt_get_probabilities_two_moves_possible_one_wins_with_delayed_expansion)
{
	const std::string boardStr = "OXOXOXX--";
	auto board = ttt::Board(boardStr);
	auto mctsCache = MonteCarloTreeSearchCacheT<ttt::Board, TicTacToeAdapter, true>(torch::kCPU, &tttAdap);
	auto mcts = MonteCarloTreeSearchT<ttt::Board, TicTacToeAdapter, true>(&mctsCache, &tttAdap, device);

	bool expansionNeeded = mcts.startSearchWithoutExpansion(board, 2, 50);
	if (expansionNeeded)
	{
		mctsCache.convertToNeuralInput();
		mctsCache.expand(&net);

		while (mcts.expandAndContinueSearchWithoutExpansion(board, 2))
		{
			mctsCache.convertToNeuralInput();
			mctsCache.expand(&net);
		}
	}

	std::vector<float> probs = getAllActionProbabilities(mcts.getProbabilities(board), tttAdap.getActionCount());

	ASSERT_GT(probs[8], probs[7]);
}

TEST(MCTS_TicTacToe, test_ttt_get_probabilities_two_moves_possible_one_gets_draw)
{
	const std::string boardStr = "OXOXXO--X";
	auto board = ttt::Board(boardStr);
	auto mctsCache = MonteCarloTreeSearchCacheT<ttt::Board, TicTacToeAdapter, true>(torch::kCPU, &tttAdap);
	auto mcts = MonteCarloTreeSearchT<ttt::Board, TicTacToeAdapter, true>(&mctsCache, &tttAdap, device);

	mcts.search(100, board, &net, 2);
	std::vector<float> probs = getAllActionProbabilities(mcts.getProbabilities(board), tttAdap.getActionCount());

	ASSERT_GT(probs[7], probs[6]);
}

#endif //RunTests
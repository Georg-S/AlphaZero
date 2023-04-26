#include <gtest/gtest.h>
#include <MonteCarloTreeSearch.h>
#include <NeuralNetworks/DefaultNeuralNet.h>
#include <TicTacToe/TicTacToeAdapter.h>
#include <Chess/ChessAdapter.h>
#include <tuple>
#include <thread>
#include <vector>
#include <MultiThreadingNeuralNetManager.h>
#include <MonteCarloTreeSearchTemplate.h>

#include "TestConfig.h"
#include "Test_Utility.h"

#if RunTests

torch::DeviceType device = torch::kCPU;
static TicTacToeAdapter tttAdap = TicTacToeAdapter();
static ChessAdapter chessAdap = ChessAdapter();

static std::vector<float> getAllActionProbabilities(const std::vector<std::pair<int, float>>& probab, size_t actionCount)
{
	auto probs = std::vector<float>(actionCount, 0.f);
	for (const auto& [action, prob] : probab)
		probs[action] = prob;

	return probs;
}

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
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(9, device, &tttAdap);
	DefaultNeuralNet net(2, 3, 3, 9);
	int result = mcts.search("OXXXXOOOX", &net, 1);

	ASSERT_EQ(result, 0);
}

TEST(MonteCarloTreeSearch, test_tt_template_mcts_t)
{
	DefaultNeuralNet net(2, 3, 3, 9);
	auto mctsCache = MonteCarloTreeSearchCacheT<ttt::Board, TicTacToeAdapter>(torch::kCPU, &tttAdap);
	auto mcts = MonteCarloTreeSearchT<ttt::Board, TicTacToeAdapter>(&mctsCache, &tttAdap, device);
	auto board = ttt::Board("XOXOXOOXX");
	float result = mcts.search(board, &net, 1);

	ASSERT_FLOAT_EQ(result, -1);
}


TEST(MonteCarloTreeSearch, test_mcts_ttt_player_one_wins)
{
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(9, device, &tttAdap);
	DefaultNeuralNet net(2, 3, 3, 9);
	float result = mcts.search("XOXOXOOXX", &net, 1);

	ASSERT_FLOAT_EQ(result, -1);
}

TEST(MonteCarloTreeSearch, test_mcts_ttt_player_two_wins)
{
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(9, torch::kCPU, &tttAdap);
	DefaultNeuralNet net(2, 3, 3, 9);
	float result = mcts.search("XXOOOXOXX", &net, 2);

	ASSERT_FLOAT_EQ(result, -1);
}

TEST(MonteCarloTreeSearch, test_ttt_get_probabilities_one_move_possible)
{
	std::string state = "XXOO-XOXX";
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(9, device, &tttAdap);
	DefaultNeuralNet net(2, 3, 3, 9);
	mcts.search(4, state, &net, 2);
	std::vector<float> probs = getAllActionProbabilities(mcts.getProbabilities(state), tttAdap.getActionCount());

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
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(9, device, &tttAdap);
	DefaultNeuralNet net(2, 3, 3, 9);
	mcts.search(100, state, &net, 2);
	std::vector<float> probs = getAllActionProbabilities(mcts.getProbabilities(state), tttAdap.getActionCount());

	ASSERT_GT(probs[8], probs[7]);
}

TEST(MonteCarloTreeSearch, test_ttt_get_probabilities_two_moves_possible_one_wins_with_delayed_expansion)
{
	std::string state = "OXOXOXX--";
	DefaultNeuralNet net(2, 3, 3, 9);
	MonteCarloTreeSearchCache buffer = MonteCarloTreeSearchCache(device, &tttAdap);
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(9, device, &tttAdap, &buffer);

	bool expansionNeeded = mcts.startSearchWithoutExpansion(state, 2, 50);
	if (expansionNeeded)
	{
		buffer.convertToNeuralInput();
		buffer.expand(&net);

		while (mcts.expandAndContinueSearchWithoutExpansion(state, 2))
		{
			buffer.convertToNeuralInput();
			buffer.expand(&net);
		}
	}

	std::vector<float> probs = getAllActionProbabilities(mcts.getProbabilities(state), tttAdap.getActionCount());

	ASSERT_GT(probs[8], probs[7]);
}

#if RunLongTests
TEST(MonteCarloTreeSearch, test_chess_one_move_wins)
{
	torch::DeviceType device = torch::kCUDA;
	const std::string gameState = "8/8/8/p7/Pn6/KPp5/RR6/r6k w - - 0 1";
	const int winningMove = chess::getIntFromMove({ 0,6,0,7 });
	DefaultNeuralNet neuralNet = DefaultNeuralNet(14, 8, 8, 4096, device);
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(chessAdap.getActionCount(), device, &chessAdap);

	mcts.search(100, gameState, &neuralNet, static_cast<int>(ceg::PieceColor::WHITE));
	auto probabilities = mcts.getProbabilities(gameState);
	int mctsBestMove = ALZ::getBestAction(probabilities);

	ASSERT_EQ(winningMove, mctsBestMove);
}
#endif //RunLongTests

TEST(MonteCarloTreeSearch, test_ttt_get_probabilities_two_moves_possible_one_gets_draw)
{
	std::string state = "OXOXXO--X";
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(9, device, &tttAdap);
	DefaultNeuralNet net(2, 3, 3, 9);
	mcts.search(100, state, &net, 2);
	std::vector<float> probs = getAllActionProbabilities(mcts.getProbabilities(state), tttAdap.getActionCount());

	ASSERT_GT(probs[7], probs[6]);
}

#endif //RunTests
#include <gtest/gtest.h>
#include <NeuralNetworks/DefaultNeuralNet.h>
#include <ConnectFour/ConnectFourAdapter.h>
#include <vector>
#include <MonteCarloTreeSearch.h>

#include "TestConfig.h"
#include "Test_Utility.h"


using namespace cn4;

#if RunTests

static torch::DeviceType device = torch::kCPU;
static ConnectFourAdapter cn4Adap = ConnectFourAdapter();
static DefaultNeuralNet net(2, 7, 6, 7);

TEST(MCTS_ConnectFour, test_mcts_cn4_yellow_wins)
{
	const std::string boardStr = "002120000012000001000000100000000000000000";
	auto board = Board(boardStr);
	auto mctsCache = MonteCarloTreeSearchCache<Board, ConnectFourAdapter, true>(torch::kCPU, &cn4Adap);
	auto mcts = MonteCarloTreeSearch<Board, ConnectFourAdapter, true>(&mctsCache, &cn4Adap, device);
	float result = mcts.search(board, &net, static_cast<int>(PlayerColor::YELLOW));

	ASSERT_FLOAT_EQ(result, 1);
}

TEST(MCTS_ConnectFour, test_mcts_cn4_red_wins)
{
	const std::string boardStr = "012112001212000012000002000000000000000000";
	auto board = Board(boardStr);
	auto mctsCache = MonteCarloTreeSearchCache<Board, ConnectFourAdapter, true>(torch::kCPU, &cn4Adap);
	auto mcts = MonteCarloTreeSearch<Board, ConnectFourAdapter, true>(&mctsCache, &cn4Adap, device);
	float result = mcts.search(board, &net, static_cast<int>(PlayerColor::RED));

	ASSERT_FLOAT_EQ(result, 1);
}

TEST(MCTS_ConnectFour, test_mcts_cn4_two_moves_possible_one_wins)
{
	const std::string boardStr = "212111012221201212200122110021212001221100";
	auto board = Board(boardStr);
	auto mctsCache = MonteCarloTreeSearchCache<Board, ConnectFourAdapter, true>(torch::kCPU, &cn4Adap);
	auto mcts = MonteCarloTreeSearch<Board, ConnectFourAdapter, true>(&mctsCache, &cn4Adap, device);
	mcts.search(100, board, &net, static_cast<int>(PlayerColor::YELLOW));
	auto probs = getAllActionProbabilities(mcts.getProbabilities(board), cn4Adap.getActionCount());

	ASSERT_GT(probs[6], probs[5]);
}

TEST(MCTS_ConnectFour, test_mcts_cn4_seven_moves_possible_one_doesnt_lose)
{
	const std::string boardStr = "021110000020000000000000000000000000000000";
	auto board = Board(boardStr);
	auto mctsCache = MonteCarloTreeSearchCache<Board, ConnectFourAdapter, true>(torch::kCPU, &cn4Adap);
	auto mcts = MonteCarloTreeSearch<Board, ConnectFourAdapter, true>(&mctsCache, &cn4Adap, device);
	mcts.search(100, board, &net, static_cast<int>(PlayerColor::RED));
	auto bestAction = ALZ::getBestAction(mcts.getProbabilities(board));

	ASSERT_EQ(bestAction, 5);
}

TEST(MCTS_ConnectFour, test_mcts_cn4_two_moves_possible_one_loses_mock_expansion)
{
	const std::string boardStr = "212111212221221212211122111221212101221100";
	auto board = Board(boardStr);
	auto mctsCache = MonteCarloTreeSearchCache<Board, ConnectFourAdapter, true>(torch::kCPU, &cn4Adap);
	auto mcts = MonteCarloTreeSearch<Board, ConnectFourAdapter, true>(&mctsCache, &cn4Adap, device);
	mcts.search(100, board, &net, static_cast<int>(PlayerColor::RED));
	auto probs = getAllActionProbabilities(mcts.getProbabilities(board), cn4Adap.getActionCount());

	ASSERT_GT(probs[5], probs[6]);
}

TEST(MCTS_ConnectFour, test_mcts_cn4_two_moves_possible_one_loses_real_expansion)
{
	const std::string boardStr = "212111212221221212211122111221212101221100";
	auto board = Board(boardStr);
	auto mctsCache = MonteCarloTreeSearchCache<Board, ConnectFourAdapter, false>(torch::kCPU, &cn4Adap);
	auto mcts = MonteCarloTreeSearch<Board, ConnectFourAdapter, false>(&mctsCache, &cn4Adap, device);
	mcts.search(100, board, &net, static_cast<int>(PlayerColor::RED));
	auto probs = getAllActionProbabilities(mcts.getProbabilities(board), cn4Adap.getActionCount());

	ASSERT_GT(probs[5], probs[6]);
}

#endif //RunTests

#include <gtest/gtest.h>
#include <MonteCarloTreeSearch.h>
#include <NeuralNetworks/DefaultNeuralNet.h>
#include <Chess/ChessAdapter.h>
#include <tuple>
#include <thread>
#include <vector>

#include "TestConfig.h"
#include "Test_Utility.h"

#if RunTests

static torch::DeviceType device = torch::kCUDA;
static ChessAdapter chessAdap = ChessAdapter();
static DefaultNeuralNet neuralNet = DefaultNeuralNet(14, 8, 8, 4096, device);

static void debugChessMoves(ChessAdapter::GameState state, std::initializer_list<int> moves)
{
	renderBoard(state.board);

	for (const auto& move : moves)
	{
		state = chessAdap.makeMove(state, move, state.currentPlayer);
		//chessAdap.getNextPlayer(state.currentPlayer);
		renderBoard(state.board);
	}
}

TEST(MCTS_Chess, test_chess_one_move_wins_white_mock_expansion)
{
	const std::string boardStr = "8/8/8/p7/Pn6/KPp5/RR6/r6k w - - 0 1";
	const ceg::BitBoard board(boardStr);
	auto gameState = ChessAdapter::GameState(board, static_cast<int>(ceg::PieceColor::WHITE));

	const int winningMove = chess::getIntFromMove({ 0,6,0,7 });
	auto mctsCache = MonteCarloTreeSearchCache<ChessAdapter::GameState, ChessAdapter, true>(device, &chessAdap);
	auto mcts = MonteCarloTreeSearch<ChessAdapter::GameState, ChessAdapter, true>(&mctsCache, &chessAdap, device);
	mcts.search(100, gameState, &neuralNet, static_cast<int>(ceg::PieceColor::WHITE));
	auto probabilities = mcts.getProbabilities(gameState);
	int mctsBestMove = ALZ::getBestAction(probabilities);

	ASSERT_EQ(winningMove, mctsBestMove);
}

TEST(MCTS_Chess, test_chess_one_move_wins_white_real_expansion)
{
	const std::string boardStr = "8/8/8/p7/Pn6/KPp5/RR6/r6k w - - 0 1";
	const ceg::BitBoard board(boardStr);
	auto gameState = ChessAdapter::GameState(board, static_cast<int>(ceg::PieceColor::WHITE));

	const int winningMove = chess::getIntFromMove({ 0,6,0,7 });
	auto mctsCache = MonteCarloTreeSearchCache<ChessAdapter::GameState, ChessAdapter, false>(device, &chessAdap);
	auto mcts = MonteCarloTreeSearch<ChessAdapter::GameState, ChessAdapter, false>(&mctsCache, &chessAdap, device);
	mcts.search(100, gameState, &neuralNet, static_cast<int>(ceg::PieceColor::WHITE));
	auto probabilities = mcts.getProbabilities(gameState);
	int mctsBestMove = ALZ::getBestAction(probabilities);

	ASSERT_EQ(winningMove, mctsBestMove);
}

TEST(MCTS_Chess, test_chess_one_move_wins_black)
{
	const std::string boardStr = "1K6/7r/8/8/8/8/8/6kq b - - 0 1";
	const ceg::BitBoard board(boardStr);
	auto gameState = ChessAdapter::GameState(board, static_cast<int>(ceg::PieceColor::BLACK));

	const int winningMove = chess::getIntFromMove({ 7,7,1,1 });
	auto mctsCache = MonteCarloTreeSearchCache<ChessAdapter::GameState, ChessAdapter, true>(device, &chessAdap);
	auto mcts = MonteCarloTreeSearch<ChessAdapter::GameState, ChessAdapter, true>(&mctsCache, &chessAdap, device);
	mcts.search(100, gameState, &neuralNet, static_cast<int>(ceg::PieceColor::BLACK));
	auto probabilities = mcts.getProbabilities(gameState);
	int mctsBestMove = ALZ::getBestAction(probabilities);

	ASSERT_EQ(winningMove, mctsBestMove);
}

TEST(MCTS_Chess, test_chess_check_black_one_move_wins_2)
{
	const std::string boardStr = "7K/r7/1r6/8/8/8/8/k7 b - - 0 1";
	const ceg::BitBoard board(boardStr);
	auto gameState = ChessAdapter::GameState(board, static_cast<int>(ceg::PieceColor::BLACK));

	const int winningMove = chess::getIntFromMove({ 1,2,1,0 });
	auto mctsCache = MonteCarloTreeSearchCache<ChessAdapter::GameState, ChessAdapter, true>(device, &chessAdap);
	auto mcts = MonteCarloTreeSearch<ChessAdapter::GameState, ChessAdapter, true>(&mctsCache, &chessAdap, device);
	mcts.search(100, gameState, &neuralNet, static_cast<int>(ceg::PieceColor::BLACK));
	auto probabilities = mcts.getProbabilities(gameState);
	int mctsBestMove = ALZ::getBestAction(probabilities);

	ASSERT_EQ(winningMove, mctsBestMove);
}

TEST(MCTS_Chess, test_chess_check_one_move_loses)
{
	const std::string boardStr = "8/6rK/r7/8/8/8/8/k7 w - - 0 1";
	const ceg::BitBoard board(boardStr);
	auto gameState = ChessAdapter::GameState(board, static_cast<int>(ceg::PieceColor::WHITE));

	const int winningMove = chess::getIntFromMove({ 7,1,6,1 });
	auto mctsCache = MonteCarloTreeSearchCache<ChessAdapter::GameState, ChessAdapter, true>(device, &chessAdap);
	auto mcts = MonteCarloTreeSearch<ChessAdapter::GameState, ChessAdapter, true>(&mctsCache, &chessAdap, device);
	mcts.search(1000, gameState, &neuralNet, static_cast<int>(ceg::PieceColor::WHITE));
	auto probabilities = mcts.getProbabilities(gameState);
	int mctsBestMove = ALZ::getBestAction(probabilities);

	ASSERT_EQ(winningMove, mctsBestMove);
}

TEST(MCTS_Chess, test_chess_check_one_move_loses_with_but_perform_other_color_mcts_before)
{
	const std::string boardStr = "8/7K/r7/8/8/6r1/8/k7 b - - 0 1";
	const ceg::BitBoard board(boardStr);
	auto gameState = ChessAdapter::GameState(board, static_cast<int>(ceg::PieceColor::BLACK));

	auto mctsCache = MonteCarloTreeSearchCache<ChessAdapter::GameState, ChessAdapter, true>(device, &chessAdap);
	auto mcts = MonteCarloTreeSearch<ChessAdapter::GameState, ChessAdapter, true>(&mctsCache, &chessAdap, device);
	mcts.search(1000, gameState, &neuralNet, static_cast<int>(ceg::PieceColor::BLACK));

	const std::string boardStr2 = "8/6rK/r7/8/8/8/8/k7 w - - 0 1";
	const ceg::BitBoard board2(boardStr2);
	auto gameState2 = ChessAdapter::GameState(board2, static_cast<int>(ceg::PieceColor::WHITE));

	const int winningMove = chess::getIntFromMove({ 7,1,6,1 });
	mcts.search(1000, gameState2, &neuralNet, static_cast<int>(ceg::PieceColor::WHITE));
	auto probabilities = mcts.getProbabilities(gameState2);
	int mctsBestMove = ALZ::getBestAction(probabilities);

	ASSERT_EQ(winningMove, mctsBestMove);
}

#if RunLongTests

TEST(MCTS_Chess, test_chess_check_mate_in_two)
{
	const std::string boardStr = "8/7K/1r6/r7/8/8/8/k7 b - - 0 1";
	const ceg::BitBoard board(boardStr);
	auto gameState = ChessAdapter::GameState(board, static_cast<int>(ceg::PieceColor::BLACK));

	const int winningMove = chess::getIntFromMove({ 0,3,0,1 });
	const int otherMove = chess::getIntFromMove({1,2,1,0});
	auto mctsCache = MonteCarloTreeSearchCache<ChessAdapter::GameState, ChessAdapter, true>(device, &chessAdap);
	auto mcts = MonteCarloTreeSearch<ChessAdapter::GameState, ChessAdapter, true>(&mctsCache, &chessAdap, device);
	mcts.search(80000, gameState, &neuralNet, static_cast<int>(ceg::PieceColor::BLACK));
	auto probabilities = mcts.getProbabilities(gameState);
	int mctsBestMove = ALZ::getBestAction(probabilities);

	ASSERT_EQ(winningMove, mctsBestMove);
}

#endif //RunLongTests

#endif //RunTests
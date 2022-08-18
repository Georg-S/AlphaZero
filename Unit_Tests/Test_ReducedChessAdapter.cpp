#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <Chess/Engine/BitBoard.h>
#include <Chess/ReducedChessAdapter.h>
#include <MonteCarloTreeSearch.h>
#include <NeuralNetworks/DefaultNeuralNet.h>

static ReducedChessAdapter adap = ReducedChessAdapter();
static ceg::ChessEngine chessEngine = ceg::ChessEngine();

TEST(ReducedChessAdapter, test_conversion_to_string_and_back_doesnt_change_board_positions)
{
	ceg::BitBoard board = ceg::BitBoard(adap.getInitialGameState());
	chessEngine.make_move(board, ceg::Move(1, 1, 1, 3));
	
	auto before = ceg::to_FEN_string(board, true);
	board = ceg::BitBoard(before);
	auto after = ceg::to_FEN_string(board, true);

	ASSERT_EQ(before, after);
}

TEST(ReducedChessAdapter, test_get_initial_state)
{
	ceg::BitBoard board = ceg::BitBoard(adap.getInitialGameState());
	std::string converted = ceg::to_FEN_string(board, false);

	ASSERT_EQ(converted, adap.getInitialGameState());
}


TEST(ReducedChessAdapter, test_get_all_possible_moves_returns_right_amount_of_moves_in_beginning)
{
	std::vector<int> possibleMoves = adap.getAllPossibleMoves(adap.getInitialGameState(), 1);

	ASSERT_EQ(possibleMoves.size(), 20);
}

TEST(ReducedChessAdapter, test_get_next_Player_for_Player_one)
{
	int player = static_cast<int>(ceg::PieceColor::WHITE);

	ASSERT_EQ(adap.getNextPlayer(player), static_cast<int>(ceg::PieceColor::BLACK));
}

TEST(ReducedChessAdapter, test_get_next_Player_for_Player_two)
{
	int player = static_cast<int>(ceg::PieceColor::BLACK);

	ASSERT_EQ(adap.getNextPlayer(player), static_cast<int>(ceg::PieceColor::WHITE));
}

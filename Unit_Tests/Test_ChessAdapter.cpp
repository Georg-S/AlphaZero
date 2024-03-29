#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <Chess/Engine/BitBoard.h>
#include <Chess/ChessAdapter.h>
#include <NeuralNetworks/DefaultNeuralNet.h>

#include "TestConfig.h"
#include "Test_Utility.h"


#if RunTests

static ChessAdapter adap = ChessAdapter();
static ceg::ChessEngine chessEngine = ceg::ChessEngine();

TEST(ChessAdapter, test_conversion_to_string_and_back_doesnt_change_board_positions)
{
	auto state = adap.getInitialGameState();
	chessEngine.make_move(state.board, ceg::Move(1, 1, 1, 3));

	auto before = ceg::to_FEN_string(state.board, true);
	state.board = ceg::BitBoard(before);
	auto after = ceg::to_FEN_string(state.board, true);

	ASSERT_EQ(before, after);
}

TEST(ChessAdapter, test_get_initial_state)
{
	auto state = adap.getInitialGameState();
	std::string converted = ceg::to_FEN_string(state.board, false);

	ASSERT_EQ(converted, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -");
}

TEST(ChessAdapter, test_get_all_possible_moves_returns_right_amount_of_moves_in_beginning)
{
	std::vector<int> possibleMoves = adap.getAllPossibleMoves(adap.getInitialGameState(), 1);

	ASSERT_EQ(possibleMoves.size(), 20);
}

TEST(ChessAdapter, test_get_next_Player_for_Player_one)
{
	int player = static_cast<int>(ceg::PieceColor::WHITE);

	ASSERT_EQ(adap.getNextPlayer(player), static_cast<int>(ceg::PieceColor::BLACK));
}

TEST(ChessAdapter, test_get_next_Player_for_Player_two)
{
	int player = static_cast<int>(ceg::PieceColor::BLACK);

	ASSERT_EQ(adap.getNextPlayer(player), static_cast<int>(ceg::PieceColor::WHITE));
}

TEST(ChessAdapter, test_make_move_1)
{
	const std::string boardStr = "rnbqkbnr/pp1ppppp/8/2p5/3P4/8/PPP1PPPP/RNBQKBNR w KQkq -";
	const int color = static_cast<int>(ceg::PieceColor::WHITE);
	ceg::BitBoard board(boardStr);
	auto state = ChessAdapter::GameState(board, color);
	int iMove = chess::getIntFromMove(ceg::Move(3, 4, 2, 3));

	auto res = adap.makeMove(state, iMove, color);
	auto resultStr = ceg::to_FEN_string(res.board, res.currentPlayer == static_cast<int>(ceg::PieceColor::BLACK));

	ASSERT_EQ(resultStr, "rnbqkbnr/pp1ppppp/8/2P5/8/8/PPP1PPPP/RNBQKBNR b KQkq -");
}

TEST(ChessAdapter, test_get_player_won_black)
{
	const std::string boardStr = "3k4/8/8/8/8/8/6r1/3K3r w - -";
	const int color = static_cast<int>(ceg::PieceColor::WHITE);
	ceg::BitBoard board(boardStr);
	auto state = ChessAdapter::GameState(board, color);
	int playerWon = adap.getPlayerWon(state);

	ASSERT_EQ(playerWon, static_cast<int>(ceg::PieceColor::BLACK));
}

TEST(ChessAdapter, test_get_player_won_white)
{
	const std::string boardStr = "3k4/3Q4/4K3/8/8/8/8/8 b - -";
	const int color = static_cast<int>(ceg::PieceColor::BLACK);
	ceg::BitBoard board(boardStr);
	auto state = ChessAdapter::GameState(board, color);
	int playerWon = adap.getPlayerWon(state);

	ASSERT_EQ(playerWon, static_cast<int>(ceg::PieceColor::WHITE));
}

TEST(ChessAdapter, test_get_player_won_none)
{
	const std::string boardStr = "3k4/8/4K3/3Q4/8/8/8/8 b - -";
	const int color = static_cast<int>(ceg::PieceColor::BLACK);
	ceg::BitBoard board(boardStr);
	auto state = ChessAdapter::GameState(board, color);
	int playerWon = adap.getPlayerWon(state);

	ASSERT_EQ(playerWon, static_cast<int>(ceg::PieceColor::NONE));
}

TEST(ChessAdapter, test_is_game_over)
{
	const std::string boardStr = "3k4/2R1R3/8/8/8/8/8/5K2 b - -";
	const int color = static_cast<int>(ceg::PieceColor::BLACK);
	ceg::BitBoard board(boardStr);
	auto state = ChessAdapter::GameState(board, color);

	ASSERT_EQ(adap.isGameOver(state), true);
}

#endif //RunTests
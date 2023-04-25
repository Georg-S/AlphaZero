#include <gtest/gtest.h>
#include <TicTacToe/TicTacToeAdapter.h>
#include <TicTacToe/Renderer.h>
#include <string>
#include "TestConfig.h"
#include "Test_Utility.h"

using namespace ttt;

TicTacToeAdapter adap = TicTacToeAdapter();

static void renderBoard(const std::string& boardStr) 
{
	renderBoard(Board(boardStr));
}

#if RunTests

TEST(TicTacToeAdapter, test_getNextPlayer)
{
	ASSERT_EQ(adap.getNextPlayer(1), 2);
	ASSERT_EQ(adap.getNextPlayer(2), 1);
}

TEST(TicTacToeAdapter, test_get_initial_player)
{
	ASSERT_EQ(adap.getInitialPlayer(), 1);
}

TEST(TicTacToeAdapter, test_get_player_won_draw)
{
	std::string state = "OXXXXOOOX";

	ASSERT_EQ(adap.getPlayerWon(state), 0);
}

TEST(TicTacToeAdapter, test_get_player_won_player_one_horizontal)
{
	std::string state = "XXXOO----";

	ASSERT_EQ(adap.getPlayerWon(state), 1);
}

TEST(TicTacToeAdapter, test_get_player_won_player_two_diagonal)
{
	std::string state = "X-OXO-O--";

	ASSERT_EQ(adap.getPlayerWon(state), 2);
}

TEST(TicTacToeAdapter, test_get_player_won_player_two_vertical)
{
	std::string state = "O-XO-XOX-";

	ASSERT_EQ(adap.getPlayerWon(state), 2);
}

TEST(TicTacToeAdapter, test_get_player_won_player_two_board_full)
{
	std::string state = "XXOXOOOXX";

	ASSERT_EQ(adap.getPlayerWon(state), 2);
}

TEST(TicTacToeAdapter, test_game_over_reward_draw_player_one)
{
	std::string state = "OXXXXOOOX";

	ASSERT_EQ(adap.gameOverReward(state, 1), 0);
}

TEST(TicTacToeAdapter, test_game_over_reward_draw_player_two)
{
	std::string state = "OXXXXOOOX";

	ASSERT_EQ(adap.gameOverReward(state, 2), 0);
}

TEST(TicTacToeAdapter, test_game_over_reward_player_one_wins)
{
	std::string state = "X-OX-OX--";

	ASSERT_EQ(adap.gameOverReward(state, 1), 1);
}

TEST(TicTacToeAdapter, test_game_over_reward_player_two_wins)
{
	std::string state = "OX--OXX-O";

	ASSERT_EQ(adap.gameOverReward(state, 2), 1);
}

TEST(TicTacToeAdapter, test_game_over_reward_player_one_loses_board_full)
{
	std::string state = "XXOXOOOXX";

	ASSERT_EQ(adap.gameOverReward(state, 1), -1);
}

TEST(TicTacToeAdapter, test_convert_state_to_neural_input_player_one)
{
	std::string state = "X-------O";

	auto input = adap.convertStateToNeuralNetInput(state, 1);
	input = input[0];

	auto plane1 = input[0];
	auto plane2 = input[1];
	float value1 = *(plane1[0][0].data_ptr<float>());
	float value2 = *(plane2[2][2].data_ptr<float>());


	ASSERT_FLOAT_EQ(value1, 1);
	ASSERT_FLOAT_EQ(value2, 1);
}

TEST(TicTacToeAdapter, test_convert_state_to_neural_input_player_two)
{
	std::string state = "X-O--X--O";

	auto input = adap.convertStateToNeuralNetInput(state, 2);
	input = input[0];

	auto plane1 = input[0];
	auto plane2 = input[1];

	float value1 = *(plane1[2][0].data_ptr<float>());
	float value2 = *(plane1[2][2].data_ptr<float>());
	float value3 = *(plane2[0][0].data_ptr<float>());
	float value4 = *(plane2[2][1].data_ptr<float>());


	ASSERT_FLOAT_EQ(value1, 1);
	ASSERT_FLOAT_EQ(value2, 1);
	ASSERT_FLOAT_EQ(value3, 1);
	ASSERT_FLOAT_EQ(value4, 1);
}

TEST(TicTacToeAdapter, test_get_all_possible_moves)
{
	std::vector<int> moves = adap.getAllPossibleMoves("-X-------", 1);

	ASSERT_EQ(moves.size(), 8);
	ASSERT_EQ(moves[1], 2);
	ASSERT_EQ(moves[2], 3);
	ASSERT_EQ(moves[3], 4);
	ASSERT_EQ(moves[4], 5);
	ASSERT_EQ(moves[5], 6);
	ASSERT_EQ(moves[6], 7);
	ASSERT_EQ(moves[7], 8);
}

TEST(TicTacToeAdapter, test_get_all_possible_moves_board_full)
{
	std::vector<int> moves = adap.getAllPossibleMoves("XOXOXOXOX", 1);

	ASSERT_EQ(moves.size(), 0);
}

TEST(TicTacToeAdapter, test_is_game_over_draw)
{
	bool gameOver = adap.isGameOver("OXXXXOOOX");

	ASSERT_EQ(gameOver, true);
}

TEST(TicTacToeAdapter, test_is_game_over_false)
{
	bool gameOver = adap.isGameOver("OXXXXOOO-");

	ASSERT_EQ(gameOver, false);
}

TEST(TicTacToeAdapter, test_is_game_over_player_one_won)
{
	bool gameOver = adap.isGameOver("--XOOX--X");

	ASSERT_EQ(gameOver, true);
}

TEST(TicTacToeAdapter, test_is_game_over_player_two_won)
{
	bool gameOver = adap.isGameOver("X-O-OXO-X");

	ASSERT_EQ(gameOver, true);
}

TEST(TicTacToeAdapter, test_make_move)
{
	std::string state = "-X-O--X--";
	state = adap.makeMove(state, 2, 2);

	ASSERT_EQ(state, "-XOO--X--");
}

TEST(TicTacToeAdapter, test_make_invalid_move)
{
	std::string state = "-X-O--X--";
	state = adap.makeMove(state, 3, 2);

	ASSERT_EQ(state, "-X-O--X--");
}

TEST(TicTacToeAdapter, test_convert_string_to_board)
{
	Board board = Board("-X-O--XOX");

	ASSERT_EQ(board.at(0,0), PlayerColor::NONE);
	ASSERT_EQ(board.at(1,0), PlayerColor::CROSS);
	ASSERT_EQ(board.at(0,1), PlayerColor::DOT);
	ASSERT_EQ(board.at(0,2), PlayerColor::CROSS);
	ASSERT_EQ(board.at(1,2), PlayerColor::DOT);
	ASSERT_EQ(board.at(2,2), PlayerColor::CROSS);
}						  

TEST(TicTacToeAdapter, test_convert_board_to_string)
{
	Board board = Board();
	board.makeMove(Move(1, 0), PlayerColor::CROSS);
	board.makeMove(Move(2, 0), PlayerColor::DOT);
	board.makeMove(Move(2, 2), PlayerColor::CROSS);

	ASSERT_EQ(board.toString(), "-XO-----X");
}

TEST(TicTacToeAdapter, test_convert_string_to_board_and_back)
{
	std::string state = "XOXOXOXOX";
	Board board = Board(state);
	std::string converted = board.toString();

	ASSERT_EQ(state, converted);
}

#endif //RunTests

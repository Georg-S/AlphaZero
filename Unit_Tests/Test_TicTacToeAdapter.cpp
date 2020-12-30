#include <gtest/gtest.h>
#include <TicTacToe/TicTacToeAdapter.h>
#include <string>


TEST(TicTacToeAdapter, test_getNextPlayer)
{
	TicTacToeAdapter adap = TicTacToeAdapter();

	ASSERT_EQ(adap.getNextPlayer(1), 2);
	ASSERT_EQ(adap.getNextPlayer(2), 1);
}

TEST(TicTacToeAdapter, test_get_initial_player)
{
	TicTacToeAdapter adap = TicTacToeAdapter();

	ASSERT_EQ(adap.getInitialPlayer(), 1);
}

TEST(TicTacToeAdapter, test_get_player_won_draw)
{
	TicTacToeAdapter adap = TicTacToeAdapter();
	std::string state = "211112221";

	ASSERT_EQ(adap.getPlayerWon(state), 0);
}

TEST(TicTacToeAdapter, test_get_player_won_player_one_horizontal)
{
	TicTacToeAdapter adap = TicTacToeAdapter();
	std::string state = "111220000";

	ASSERT_EQ(adap.getPlayerWon(state), 1);
}

TEST(TicTacToeAdapter, test_get_player_won_player_two_diagonal)
{
	TicTacToeAdapter adap = TicTacToeAdapter();
	std::string state = "102120200";

	ASSERT_EQ(adap.getPlayerWon(state), 2);
}

TEST(TicTacToeAdapter, test_get_player_won_player_two_vertical)
{
	TicTacToeAdapter adap = TicTacToeAdapter();
	std::string state = "201201210";

	ASSERT_EQ(adap.getPlayerWon(state), 2);
}

TEST(TicTacToeAdapter, test_get_player_won_player_two_board_full)
{
	TicTacToeAdapter adap = TicTacToeAdapter();
	std::string state = "112122211";

	ASSERT_EQ(adap.getPlayerWon(state), 2);
}

TEST(TicTacToeAdapter, test_game_over_reward_draw_player_one)
{
	TicTacToeAdapter adap = TicTacToeAdapter();
	std::string state = "211112221";

	ASSERT_EQ(adap.gameOverReward(state, 1), 0);
}

TEST(TicTacToeAdapter, test_game_over_reward_draw_player_two)
{
	TicTacToeAdapter adap = TicTacToeAdapter();
	std::string state = "211112221";

	ASSERT_EQ(adap.gameOverReward(state, 2), 0);
}

TEST(TicTacToeAdapter, test_game_over_reward_player_one_wins)
{
	TicTacToeAdapter adap = TicTacToeAdapter();
	std::string state = "102102100";

	ASSERT_EQ(adap.gameOverReward(state, 1), 1);
}

TEST(TicTacToeAdapter, test_game_over_reward_player_two_wins)
{
	TicTacToeAdapter adap = TicTacToeAdapter();
	std::string state = "210021102";

	ASSERT_EQ(adap.gameOverReward(state, 2), 1);
}

TEST(TicTacToeAdapter, test_game_over_reward_player_one_loses_board_full)
{
	TicTacToeAdapter adap = TicTacToeAdapter();
	std::string state = "112122211";

	ASSERT_EQ(adap.gameOverReward(state, 1), -1);
}

TEST(TicTacToeAdapter, test_convert_state_to_neural_input_player_one)
{
	TicTacToeAdapter adap = TicTacToeAdapter();
	std::string state = "100000002";

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
	TicTacToeAdapter adap = TicTacToeAdapter();
	std::string state = "102001002";

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
	TicTacToeAdapter adap = TicTacToeAdapter();
	std::vector<int> moves = adap.getAllPossibleMoves("010000000", 1);

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
	TicTacToeAdapter adap = TicTacToeAdapter();
	std::vector<int> moves = adap.getAllPossibleMoves("121212121", 1);

	ASSERT_EQ(moves.size(), 0);
}

TEST(TicTacToeAdapter, test_is_game_over_draw)
{
	TicTacToeAdapter adap = TicTacToeAdapter();
	bool gameOver = adap.isGameOver("211112221");

	ASSERT_EQ(gameOver, true);
}

TEST(TicTacToeAdapter, test_is_game_over_false)
{
	TicTacToeAdapter adap = TicTacToeAdapter();
	bool gameOver = adap.isGameOver("211112220");

	ASSERT_EQ(gameOver, false);
}

TEST(TicTacToeAdapter, test_is_game_over_player_one_won)
{
	TicTacToeAdapter adap = TicTacToeAdapter();
	bool gameOver = adap.isGameOver("001221001");

	ASSERT_EQ(gameOver, true);
}

TEST(TicTacToeAdapter, test_is_game_over_player_two_won)
{
	TicTacToeAdapter adap = TicTacToeAdapter();
	bool gameOver = adap.isGameOver("102021201");

	ASSERT_EQ(gameOver, true);
}

TEST(TicTacToeAdapter, test_make_move)
{
	TicTacToeAdapter adap = TicTacToeAdapter();
	std::string state = "010200100";
	state = adap.makeMove(state, 2, 2);

	ASSERT_EQ(state, "012200100");
}

TEST(TicTacToeAdapter, test_make_invalid_move)
{
	TicTacToeAdapter adap = TicTacToeAdapter();
	std::string state = "010200100";
	state = adap.makeMove(state, 3, 2);

	ASSERT_EQ(state, "010200100");
}

TEST(TicTacToeAdapter, test_convert_string_to_board)
{
	TicTacToeAdapter adap = TicTacToeAdapter();
	ttt::Board board = adap.convertStringToBoard("010200121");

	ASSERT_EQ(board.board[0][0], 0);
	ASSERT_EQ(board.board[1][0], 1);
	ASSERT_EQ(board.board[0][1], 2);
	ASSERT_EQ(board.board[0][2], 1);
	ASSERT_EQ(board.board[1][2], 2);
	ASSERT_EQ(board.board[2][2], 1);
}

TEST(TicTacToeAdapter, test_convert_board_to_string)
{
	TicTacToeAdapter adap = TicTacToeAdapter();
	ttt::Board board = ttt::Board();
	board.board[1][0] = 1;
	board.board[2][0] = 2;
	board.board[2][2] = 1;

	ASSERT_EQ(board.toString(), "012000001");
}

TEST(TicTacToeAdapter, test_convert_string_to_board_and_back)
{
	TicTacToeAdapter adap = TicTacToeAdapter();
	std::string state = "121212121";
	ttt::Board board = adap.convertStringToBoard(state);
	std::string converted = board.toString();

	ASSERT_EQ(state, converted);
}
#include <gtest/gtest.h>
#include <tuple>
#include <map>
#include <MonteCarloTreeSearch.h>
#include <ConnectFour/ConnectFourAdapter.h>
#include <ConnectFour/ConnectFour.h>
#include <ConnectFour/Renderer.h>

using namespace cn4;

static ConnectFourAdapter adap = ConnectFourAdapter();

static void renderBoard(const Board& board)
{
	Renderer renderer = Renderer();
	while (!renderer.isQuit())
		renderer.update(board);

	renderer.quit();
}

static void renderBoard(const std::string& boardStr)
{
	renderBoard(Board(boardStr));
}

TEST(ConnectFourAdapter, test_get_player_won_draw)
{
	std::string state = "111221122112221122111221122211221112211222";

	ASSERT_EQ(adap.getPlayerWon(state), 0);
}

TEST(ConnectFourAdapter, test_get_player_won_board_full_player_one_wins)
{
	std::string state = "111221122112221122111221122211211112221222";

	ASSERT_EQ(adap.getPlayerWon(state), 1);
}

TEST(ConnectFourAdapter, test_get_player_won_returns_0_if_game_is_not_over)
{
	std::string state = "000000000000000000000000000000000000000000";

	ASSERT_EQ(adap.getPlayerWon(state), 0);
}

TEST(ConnectFourAdapter, test_get_player_won_player_one_wins_horizontal)
{
	std::string state = "111100022200000000000000000000000000000000";

	ASSERT_EQ(adap.getPlayerWon(state), 1);
}

TEST(ConnectFourAdapter, test_get_player_won_player_two_wins_vertical)
{
	std::string state = "111000210000020000002000000200000000000000";

	ASSERT_EQ(adap.getPlayerWon(state), 2);
}

TEST(ConnectFourAdapter, test_get_player_won_player_one_wins_diagonal_up)
{
	std::string state = "001222000211100000120000001000000000000000";

	ASSERT_EQ(adap.getPlayerWon(state), 1);
}

TEST(ConnectFourAdapter, test_get_player_won_player_two_wins_diagonal_down)
{
	std::string state = "000111200022210001200000200000000000000000";

	ASSERT_EQ(adap.getPlayerWon(state), 2);
}

TEST(ConnectFourAdapter, test_game_over_reward_draw)
{
	std::string state = "111221122112221122111221122211221112211222";

	ASSERT_EQ(adap.gameOverReward(state, 1), 0);
}

TEST(ConnectFourAdapter, test_game_over_reward_board_full_player_one_wins)
{
	std::string state = "111221122112221122111221122211211112221222";

	ASSERT_EQ(adap.gameOverReward(state, 1), 1);
}

TEST(ConnectFourAdapter, test_is_game_over_draw)
{
	std::string state = "111221122112221122111221122211221112211222";

	ASSERT_EQ(adap.isGameOver(state), true);
}

TEST(ConnectFourAdapter, test_is_game_over_false)
{
	std::string state = "111221122112221122111221122211221112211220";

	ASSERT_EQ(adap.isGameOver(state), false);
}

TEST(ConnectFourAdapter, test_is_game_over_player_one_wins)
{
	std::string state = "001222000211100000120000001000000000000000";

	ASSERT_EQ(adap.isGameOver(state), true);
}

TEST(ConnectFourAdapter, test_game_is_not_over)
{
	std::string state = "220121121011122101221120200221010011202002";

	ASSERT_EQ(adap.isGameOver(state), false);
}

TEST(ConnectFourAdapter, test_is_game_over_player_two_wins)
{
	std::string state = "111000210000020000002000000200000000000000";

	ASSERT_EQ(adap.isGameOver(state), true);
}

TEST(ConnectFourAdapter, test_make_move_player_one_empty_board)
{
	std::string state = "000000000000000000000000000000000000000000";
	state = adap.makeMove(state, 1, 1);

	ASSERT_EQ(state, "010000000000000000000000000000000000000000");
}

TEST(ConnectFourAdapter, test_make_move_player_two_empty_board)
{
	std::string state = "000000000000000000000000000000000000000000";
	state = adap.makeMove(state, 1, 2);

	ASSERT_EQ(state, "020000000000000000000000000000000000000000");
}

TEST(ConnectFourAdapter, test_make_move_player_one)
{
	std::string state = "121212120000000000000000000000000000000000";
	state = adap.makeMove(state, 1, 1);

	ASSERT_EQ(state, "121212121000000000000000000000000000000000");
}

TEST(ConnectFourAdapter, test_make_move_player_two)
{
	std::string state = "121212120000000000000000000000000000000000";
	state = adap.makeMove(state, 1, 2);

	ASSERT_EQ(state, "121212122000000000000000000000000000000000");
}

TEST(ConnectFourAdapter, test_conversion_to_string_and_back)
{
	std::string state = "111221122112221122111221122211221112211222";

	Board board = Board(state);
	std::string convertedState = board.toString();

	ASSERT_EQ(state, convertedState);
}

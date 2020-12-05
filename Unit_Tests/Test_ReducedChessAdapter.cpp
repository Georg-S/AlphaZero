#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <Chess/GameLogic.h>
#include <Chess/ReducedChessAdapter.h>
#include <MonteCarloTreeSearch.h>
#include <NeuralNetworks/DefaultNoResidual.h>


TEST(ReducedChessAdapter, test_conversion_to_string_and_back_doesnt_change_board_positions) {
    ReducedChessAdapter adap = ReducedChessAdapter();
    chess::Board board = chess::Board();
    board.setToInitialState();
    chess::GameLogic::makeMove(board, chess::Move(1,1,1,3));

    std::string before = adap.convertStateToString(board,1);
    board = adap.convertStateStringToBoard(before);
    std::string after = adap.convertStateToString(board,1);

    std::cout << before << std::endl;

    ASSERT_EQ(before, after);
}

TEST(ReducedChessAdapter, test_get_initial_state) {
    ReducedChessAdapter adap = ReducedChessAdapter();
    chess::Board board = chess::Board();
    board.setToInitialState();

    std::string converted = adap.convertStateToString(board,1);

    ASSERT_EQ(converted, adap.getInitialGameState());
}


TEST(ReducedChessAdapter, test_get_all_possible_moves_returns_right_amount_of_moves_in_beginning) {
    ReducedChessAdapter adap = ReducedChessAdapter();
    std::vector<int> possibleMoves = adap.getAllPossibleMoves(adap.getInitialGameState(), 1);

    ASSERT_EQ(possibleMoves.size(), 20);
}

TEST(ReducedChessAdapter, test_get_next_Player_for_Player_one) {
    ReducedChessAdapter adap = ReducedChessAdapter();
    int player = (int)chess::PieceColor::WHITE;

    ASSERT_EQ(adap.getNextPlayer(player),2);
}

TEST(ReducedChessAdapter, test_get_next_Player_for_Player_two) {
    ReducedChessAdapter adap = ReducedChessAdapter();
    int player = (int)chess::PieceColor::BLACK;

    ASSERT_EQ(adap.getNextPlayer(player),1);
}

TEST(ReducedChessAdapter, test_get_next_Player_for_Player_None) {
    ReducedChessAdapter adap = ReducedChessAdapter();
    int player = (int)chess::PieceColor::NONE;

    ASSERT_EQ(adap.getNextPlayer(player),0);
}
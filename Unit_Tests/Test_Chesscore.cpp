#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <Chess/GameLogic.h>
#include <Chess/ReducedChessAdapter.h>



TEST(Move, test_move_conversion_to_int_and_back) {
    chess::Move move = chess::Move(1,1,1,3);
    int converted = move.getSingleIntRepresentation();
    chess::Move backConv = chess::Move(converted);

    ASSERT_EQ(move.fromX, backConv.fromX);
    ASSERT_EQ(move.fromY, backConv.fromY);
    ASSERT_EQ(move.toX, backConv.toX);
    ASSERT_EQ(move.toY, backConv.toY);
}
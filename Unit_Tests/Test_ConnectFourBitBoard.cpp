#include <gtest/gtest.h>
#include <ConnectFour/BitBoard.h>
#include <ConnectFour/Board.h>
#include <ConnectFour/GameLogic.h>
#include <bitset>

/*
TEST(ConnectFourBitBoard, test_printStatements) 
{
    cn4::Board board;
    cn4::GameLogic::makeMove(board, 2, cn4::PlayerColor::YELLOW);
    cn4::GameLogic::makeMove(board, 1, cn4::PlayerColor::RED);
    cn4::GameLogic::makeMove(board, 4,  cn4::PlayerColor::YELLOW);
    board.print();

    std::cout << std::endl;
    cn4::BitBoard bitBoard = cn4::BitBoard(board,1);
    bitBoard.print();
}
*/

TEST(ConnectFourBitBoard, test_writeBit1AtPosition0) 
{
    cn4::BitBoard bitBoard = cn4::BitBoard();
    bitBoard.writeSingleBitToPositionOnBoard(0, 1);


    ASSERT_EQ(bitBoard.getCurrentBitRepresentation(), (uint64_t)1);
}


TEST(ConnectFourBitBoard, test_writeBit1AtPosition1)
{
    cn4::BitBoard bitBoard = cn4::BitBoard();
    bitBoard.writeSingleBitToPositionOnBoard(1, 1);


    ASSERT_EQ(bitBoard.getCurrentBitRepresentation(), (uint64_t)2);
}

TEST(ConnectFourBitBoard, test_writeBit1AtPosition0And1)
{
    cn4::BitBoard bitBoard = cn4::BitBoard();
    bitBoard.writeSingleBitToPositionOnBoard(0, 1);
    bitBoard.writeSingleBitToPositionOnBoard(1, 1);


    ASSERT_EQ(bitBoard.getCurrentBitRepresentation(), (uint64_t)3);
}

TEST(ConnectFourBitBoard, test_writeBit0AtPosition0)
{
    cn4::BitBoard bitBoard = cn4::BitBoard();
    bitBoard.writeSingleBitToPositionOnBoard(0, 1);
    bitBoard.writeSingleBitToPositionOnBoard(1, 1);
    bitBoard.writeSingleBitToPositionOnBoard(0, 0);


    ASSERT_EQ(bitBoard.getCurrentBitRepresentation(), (uint64_t)2);
}

TEST(ConnectFourBitBoard, test_writeColumnSizeToPosition)
{
    cn4::BitBoard bitBoard = cn4::BitBoard();
    bitBoard.writeBinaryValueToBoard(6,42);
    //110 = 6
    uint64_t binaryResult = 0b0000000000000000000110000000000000000000000000000000000000000000;


    ASSERT_EQ(bitBoard.getCurrentBitRepresentation(), binaryResult);
}

TEST(ConnectFourBitBoard, test_BitBoardConstructorFromBoardSizeColumn1Is1)
{
    cn4::Board board;
    cn4::GameLogic::makeMove(board, 0, cn4::PlayerColor::YELLOW);
    cn4::BitBoard bitBoard(board,1);
    uint64_t desiredOutput = 0b0000000000000000000001000000000000000000000000000000000000000001;
    uint64_t output = bitBoard.getCurrentBitRepresentation();

    ASSERT_EQ(output, desiredOutput);
}

TEST(ConnectFourBitBoard, test_BitBoardConstructorFromEmptyBoard)
{
    cn4::Board board;
    cn4::BitBoard bitBoard(board, 1);
    uint64_t desiredOutput = 0;
    uint64_t output = bitBoard.getCurrentBitRepresentation();

    ASSERT_EQ(output, desiredOutput);
}
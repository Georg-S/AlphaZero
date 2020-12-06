#ifndef DEEPREINFORCEMENTLEARNING_chess_PIECE_H
#define DEEPREINFORCEMENTLEARNING_chess_PIECE_H

#include <iostream>
#include "Chess/Move.h"
#include "Chess/PieceColor.h"

namespace chess {
    class Piece {
    public:
        PieceColor getPieceColor();
        char getPieceChar();
        virtual bool isMoveValid(Piece* board[8][8], const chess::Move &move) = 0;
        virtual void makeMove(Piece* board[8][8], const chess::Move &move) = 0;
        virtual Piece* getDeepCopy() = 0;
        static bool isCheck(Piece* board[8][8], const chess::PieceColor &color);
        static bool isCheck(Piece* board[8][8], const int &kingX, const int &kingY);

        bool moved = false;

    protected:
        static bool isFieldEmpty(Piece* board[8][8], const int &x, const int &y);
        static bool piecesHaveDifferentColor(Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
        static void movePieceToPosition(Piece* board[8][8], const Move &move);
        static void setEnPassantDirectionsOfOneColorToZero(Piece* board[8][8], chess::PieceColor color);
        static void debugPrintBoardToConsole(chess::Piece* board[8][8]);


        chess::PieceColor color;
        char pieceChar;
        int enPassantDirection = 0;
    };
}

#endif //DEEPREINFORCEMENTLEARNING_chess_PIECE_H

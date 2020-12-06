#ifndef DEEPREINFORCEMENTLEARNING_KING_H
#define DEEPREINFORCEMENTLEARNING_KING_H

#include <iostream>
#include <math.h>
#include "Piece.h"

class King : public chess::Piece{
public:
    King(chess::PieceColor color);
    ~King();

    bool isMoveValid(chess::Piece* board[8][8], const chess::Move &move) override;
    void makeMove(chess::Piece* board[8][8], const chess::Move &move) override;
    Piece * getDeepCopy() override;

private:
    bool normalMovePossible(chess::Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
    bool castlingPossible(chess::Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
    bool isValidRookForCastlingField(chess::Piece* board[8][8], const int &rookX, const int &rookY);
    bool testCheckOnPositionAndResetBoard(chess::Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
    bool isCastlingMove(const int &fromX, const int &toX);
    void makeCastlingMove(chess::Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);

};


#endif //DEEPREINFORCEMENTLEARNING_KING_H

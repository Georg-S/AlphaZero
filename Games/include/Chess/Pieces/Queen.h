#ifndef DEEPREINFORCEMENTLEARNING_QUEEN_H
#define DEEPREINFORCEMENTLEARNING_QUEEN_H

#include "Piece.h"

class Queen : public chess::Piece{
public:
    Queen(chess::PieceColor color);
    ~Queen();

    bool isMoveValid(Piece *(*board)[8], const chess::Move &move) override;
    void makeMove(Piece *(*board)[8], const chess::Move &move) override;
    Piece * getDeepCopy() override;

private:
    bool diagonalPossible(Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
    bool horizontalPossible(Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
    bool verticalPossible(Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);

    bool rightPossible(Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
    bool upPossible(Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
    bool leftPossible(Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
    bool downPossible(Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
    bool leftUpPossible(Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
    bool leftDownPossible(Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
    bool rightDownPossible(Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
    bool rightUpPossible(Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
};


#endif //DEEPREINFORCEMENTLEARNING_QUEEN_H

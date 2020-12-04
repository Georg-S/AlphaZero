#ifndef PROTOTYPE_BISHOP_H
#define PROTOTYPE_BISHOP_H

#include "Piece.h"

class Bishop : public chess::Piece{
public:
    Bishop(chess::PieceColor color);
    ~Bishop();

    bool isMoveValid(Piece *(*board)[8], const chess::Move &move) override;
    void makeMove(Piece *(*board)[8], const chess::Move &move) override;
    Piece * getDeepCopy() override;

private:
    bool leftUpPossible(Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
    bool leftDownPossible(Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
    bool rightDownPossible(Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
    bool rightUpPossible(Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
};


#endif //PROTOTYPE_BISHOP_H

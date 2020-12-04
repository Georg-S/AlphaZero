#ifndef PROTOTYPE_ROOK_H
#define PROTOTYPE_ROOK_H

#include "Piece.h"

class Rook : public chess::Piece{
public:
    Rook(chess::PieceColor color);
    ~Rook();

    bool isMoveValid(Piece *(*board)[8], const chess::Move &move) override;
    void makeMove(Piece *(*board)[8], const chess::Move &move) override;
    Piece * getDeepCopy() override;

private:
    bool rightPossible(Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
    bool upPossible(Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
    bool leftPossible(Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
    bool downPossible(Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
};


#endif //PROTOTYPE_ROOK_H

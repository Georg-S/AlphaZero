#ifndef PROTOTYPE_KNIGHT_H
#define PROTOTYPE_KNIGHT_H

#include <math.h>
#include "Piece.h"

class Knight : public chess::Piece{
public:
    Knight(chess::PieceColor color);
    ~Knight();

    bool isMoveValid(Piece *(*board)[8], const chess::Move &move) override;
    void makeMove(Piece *(*board)[8], const chess::Move &move) override;
    Piece * getDeepCopy() override;
};


#endif //PROTOTYPE_KNIGHT_H

#ifndef PROTOTYPE_PAWN_H
#define PROTOTYPE_PAWN_H

#include <math.h>
#include "Piece.h"

class Pawn : public chess::Piece{
public:
    Pawn(chess::PieceColor color);
    ~Pawn();

    bool isMoveValid(Piece *(*board)[8], const chess::Move &move) override;
    void makeMove(Piece *(*board)[8], const chess::Move &move) override;
    Piece * getDeepCopy() override;

private:

    bool normalMovementStrategyPossible(Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
    bool normalMovementStrategyForBlackPossible(Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
    bool normalMovementStrategyForWhitePossible(Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
    bool enPassantPossible(Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY, const int &direction);
    bool isEnPassantMove(Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
    void updateEnPassantDirection(Piece* board[8][8], const int &fromX, const int &fromY, const int &toX, const int &toY);
    void updateEnPassant(Piece* board[8][8], const int &fromX, const int &fromY, const int &updatePawnX, const int &updatePawnY);

};


#endif //PROTOTYPE_PAWN_H

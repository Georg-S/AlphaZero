#ifndef DEEPREINFORCEMENTLEARNING_ROOK_H
#define DEEPREINFORCEMENTLEARNING_ROOK_H

#include "Piece.h"

class Rook : public chess::Piece 
{
public:
	Rook(chess::PieceColor color);
	bool isMoveValid(Piece* (*board)[8], const chess::Move& move) override;
	void makeMove(Piece* (*board)[8], const chess::Move& move) override;
	Piece* getDeepCopy() override;

private:
	bool rightPossible(Piece* board[8][8], int fromX, int fromY, int toX, int toY);
	bool upPossible(Piece* board[8][8], int fromX, int fromY, int toX, int toY);
	bool leftPossible(Piece* board[8][8], int fromX, int fromY, int toX, int toY);
	bool downPossible(Piece* board[8][8], int fromX, int fromY, int toX, int toY);
};


#endif //DEEPREINFORCEMENTLEARNING_ROOK_H

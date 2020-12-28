#ifndef DEEPREINFORCEMENTLEARNING_PAWN_H
#define DEEPREINFORCEMENTLEARNING_PAWN_H

#include <math.h>
#include "Piece.h"

class Pawn : public chess::Piece 
{
public:
	Pawn(chess::PieceColor color);
	bool isMoveValid(Piece* (*board)[8], const chess::Move& move) override;
	void makeMove(Piece* (*board)[8], const chess::Move& move) override;
	Piece* getDeepCopy() override;

private:
	bool normalMovementStrategyPossible(Piece* board[8][8], int fromX, int fromY, int toX, int toY);
	bool normalMovementStrategyForBlackPossible(Piece* board[8][8], int fromX, int fromY, int toX, int toY);
	bool normalMovementStrategyForWhitePossible(Piece* board[8][8], int fromX, int fromY, int toX, int toY);
	bool enPassantPossible(Piece* board[8][8], int fromX, int fromY, int toX, int toY, int direction);
	bool isEnPassantMove(Piece* board[8][8], int fromX, int fromY, int toX, int toY);
	void updateEnPassantDirection(Piece* board[8][8], int fromX, int fromY, int toX, int toY);
	void updateEnPassant(Piece* board[8][8], int fromX, int fromY, int updatePawnX, int updatePawnY);
};


#endif //DEEPREINFORCEMENTLEARNING_PAWN_H

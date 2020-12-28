#ifndef DEEPREINFORCEMENTLEARNING_KING_H
#define DEEPREINFORCEMENTLEARNING_KING_H

#include <iostream>
#include <math.h>
#include "Piece.h"

class King : public chess::Piece
{
public:
	King(chess::PieceColor color);
	bool isMoveValid(chess::Piece* board[8][8], const chess::Move& move) override;
	void makeMove(chess::Piece* board[8][8], const chess::Move& move) override;
	Piece* getDeepCopy() override;

private:
	bool normalMovePossible(chess::Piece* board[8][8], int fromX, int fromY, int toX, int toY);
	bool castlingPossible(chess::Piece* board[8][8], int fromX, int fromY, int toX, int toY);
	bool isValidRookForCastlingField(chess::Piece* board[8][8], int rookX, int rookY);
	bool testCheckOnPositionAndResetBoard(chess::Piece* board[8][8], int fromX, int fromY, int toX, int toY);
	bool isCastlingMove(int fromX, int toX);
	void makeCastlingMove(chess::Piece* board[8][8], int fromX, int fromY, int toX, int toY);
};

#endif //DEEPREINFORCEMENTLEARNING_KING_H

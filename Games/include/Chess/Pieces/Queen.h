#ifndef DEEPREINFORCEMENTLEARNING_QUEEN_H
#define DEEPREINFORCEMENTLEARNING_QUEEN_H

#include "Piece.h"

class Queen : public chess::Piece
{
public:
	Queen(chess::PieceColor color);
	bool isMoveValid(Piece* (*board)[8], const chess::Move& move) override;
	void makeMove(Piece* (*board)[8], const chess::Move& move) override;
	Piece* getDeepCopy() override;

private:
	bool diagonalPossible(Piece* board[8][8], int fromX, int fromY, int toX, int toY);
	bool horizontalPossible(Piece* board[8][8], int fromX, int fromY, int toX, int toY);
	bool verticalPossible(Piece* board[8][8], int fromX, int fromY, int toX, int toY);

	bool rightPossible(Piece* board[8][8], int fromX, int fromY, int toX, int toY);
	bool upPossible(Piece* board[8][8], int fromX, int fromY, int toX, int toY);
	bool leftPossible(Piece* board[8][8], int fromX, int fromY, int toX, int toY);
	bool downPossible(Piece* board[8][8], int fromX, int fromY, int toX, int toY);
	bool leftUpPossible(Piece* board[8][8], int fromX, int fromY, int toX, int toY);
	bool leftDownPossible(Piece* board[8][8], int fromX, int fromY, int toX, int toY);
	bool rightDownPossible(Piece* board[8][8], int fromX, int fromY, int toX, int toY);
	bool rightUpPossible(Piece* board[8][8], int fromX, int fromY, int toX, int toY);
};


#endif //DEEPREINFORCEMENTLEARNING_QUEEN_H

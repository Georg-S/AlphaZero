#ifndef DEEPREINFORCEMENTLEARNING_BISHOP_H
#define DEEPREINFORCEMENTLEARNING_BISHOP_H

#include "Piece.h"

class Bishop : public chess::Piece {
public:
	Bishop(chess::PieceColor color);
	~Bishop();

	bool isMoveValid(Piece* (*board)[8], const chess::Move& move) override;
	void makeMove(Piece* (*board)[8], const chess::Move& move) override;
	Piece* getDeepCopy() override;

private:
	bool leftUpPossible(Piece* board[8][8], int fromX, int fromY, int toX, int toY);
	bool leftDownPossible(Piece* board[8][8], int fromX, int fromY, int toX, int toY);
	bool rightDownPossible(Piece* board[8][8], int fromX, int fromY, int toX, int toY);
	bool rightUpPossible(Piece* board[8][8], int fromX, int fromY, int toX, int toY);
};


#endif //DEEPREINFORCEMENTLEARNING_BISHOP_H

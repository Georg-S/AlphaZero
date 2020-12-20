#include "Chess/Pieces/Knight.h"

Knight::Knight(chess::PieceColor color) {
	this->color = color;
	this->pieceChar = 'H';
	this->moved = false;
}

bool Knight::isMoveValid(chess::Piece* (*board)[8], const chess::Move& move) {

	int xDistance = abs(move.fromX - move.toX);
	int yDistance = abs(move.fromY - move.toY);
	if (xDistance + yDistance == 3 && xDistance != 0 && yDistance != 0) {
		if (isFieldEmpty(board, move.toX, move.toY))
			return true;
		if (piecesHaveDifferentColor(board, move.fromX, move.fromY, move.toX, move.toY))
			return true;
	}
	return false;
}

void Knight::makeMove(chess::Piece* (*board)[8], const chess::Move& move) {
	setEnPassantDirectionsOfOneColorToZero(board, board[move.fromX][move.fromY]->getPieceColor());
	movePieceToPosition(board, move);
}

chess::Piece* Knight::getDeepCopy() {
	Knight* knight = new Knight(this->color);
	knight->moved = this->moved;

	return knight;
}

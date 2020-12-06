#include "Chess/Pieces/Bishop.h"

Bishop::Bishop(chess::PieceColor color) {
	this->color = color;
	this->pieceChar = 'B';
	this->moved = false;
}

Bishop::~Bishop() {

}

bool Bishop::isMoveValid(chess::Piece* (*board)[8], const chess::Move& move) {

	if (move.fromX == move.toX || move.toY == move.fromY)
		return false;

	return leftUpPossible(board, move.fromX, move.fromY, move.toX, move.toY) ||
		rightUpPossible(board, move.fromX, move.fromY, move.toX, move.toY) ||
		leftDownPossible(board, move.fromX, move.fromY, move.toX, move.toY) ||
		rightDownPossible(board, move.fromX, move.fromY, move.toX, move.toY);
}

void Bishop::makeMove(chess::Piece* (*board)[8], const chess::Move& move) {
	setEnPassantDirectionsOfOneColorToZero(board, this->getPieceColor());
	movePieceToPosition(board, move);
}

bool
Bishop::leftUpPossible(chess::Piece* (*board)[8], int fromX, int fromY, int toX, int toY) {
	for (int x = fromX - 1, y = fromY - 1; x >= 0 && y >= 0; x--, y--) {
		if (x == toX && y == toY) {
			if (isFieldEmpty(board, toX, toY))
				return true;

			if (piecesHaveDifferentColor(board, fromX, fromY, toX, toY))
				return true;

			return false;
		}
		if (!isFieldEmpty(board, x, y))
			return false;
	}
	return false;
}

bool Bishop::leftDownPossible(chess::Piece* (*board)[8], int fromX, int fromY, int toX, int toY) {
	for (int x = fromX - 1, y = fromY + 1; x >= 0 && y < 8; x--, y++) {
		if (x == toX && y == toY) {
			if (isFieldEmpty(board, toX, toY))
				return true;

			if (piecesHaveDifferentColor(board, fromX, fromY, toX, toY))
				return true;

			return false;
		}
		if (!isFieldEmpty(board, x, y))
			return false;
	}
	return false;
}

bool Bishop::rightDownPossible(chess::Piece* (*board)[8], int fromX, int fromY, int toX, int toY) {
	for (int x = fromX + 1, y = fromY + 1; x < 8 && y < 8; x++, y++) {
		if (x == toX && y == toY) {
			if (isFieldEmpty(board, toX, toY))
				return true;

			if (piecesHaveDifferentColor(board, fromX, fromY, toX, toY))
				return true;

			return false;
		}
		if (!isFieldEmpty(board, x, y))
			return false;
	}
	return false;
}

bool
Bishop::rightUpPossible(chess::Piece* (*board)[8], int fromX, int fromY, int toX, int toY) {
	for (int x = fromX + 1, y = fromY - 1; x < 8 && y >= 0; x++, y--) {
		if (x == toX && y == toY) {
			if (isFieldEmpty(board, toX, toY))
				return true;

			if (piecesHaveDifferentColor(board, fromX, fromY, toX, toY))
				return true;

			return false;
		}
		if (!isFieldEmpty(board, x, y))
			return false;
	}
	return false;
}

chess::Piece* Bishop::getDeepCopy() {
	Bishop* bishop = new Bishop(this->color);
	bishop->moved = this->moved;

	return bishop;
}

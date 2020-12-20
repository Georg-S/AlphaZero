#include "Chess/Pieces/Rook.h"

Rook::Rook(chess::PieceColor color) {
	this->color = color;
	this->pieceChar = 'R';
	this->moved = false;
}

bool Rook::isMoveValid(chess::Piece* (*board)[8], const chess::Move& move) {

	if (move.fromX != move.toX && move.fromY != move.toY)
		return false;


	return leftPossible(board, move.fromX, move.fromY, move.toX, move.toY) ||
		rightPossible(board, move.fromX, move.fromY, move.toX, move.toY) ||
		upPossible(board, move.fromX, move.fromY, move.toX, move.toY) ||
		downPossible(board, move.fromX, move.fromY, move.toX, move.toY);
}

void Rook::makeMove(chess::Piece* (*board)[8], const chess::Move& move) {
	setEnPassantDirectionsOfOneColorToZero(board, board[move.fromX][move.fromY]->getPieceColor());
	movePieceToPosition(board, move);
}

bool
Rook::rightPossible(chess::Piece* (*board)[8], int fromX, int fromY, int toX, int toY) {
	if (fromX >= toX)
		return false;

	for (int x = fromX + 1; x < 8; x++) {
		if (x == toX && fromY == toY) {
			if (isFieldEmpty(board, toX, toY))
				return true;

			if (piecesHaveDifferentColor(board, fromX, fromY, toX, toY))
				return true;

			return false;
		}
		if (!isFieldEmpty(board, x, fromY))
			return false;
	}
	return false;
}

bool Rook::upPossible(chess::Piece* (*board)[8], int fromX, int fromY, int toX, int toY) {
	for (int y = fromY - 1; y >= 0; y--) {
		if (fromX == toX && y == toY) {
			if (isFieldEmpty(board, toX, toY))
				return true;

			if (piecesHaveDifferentColor(board, fromX, fromY, toX, toY))
				return true;

			return false;
		}
		if (!isFieldEmpty(board, fromX, y))
			return false;
	}
	return false;
}

bool Rook::leftPossible(chess::Piece* (*board)[8], int fromX, int fromY, int toX, int toY) {
	if (fromX <= toX)
		return false;

	for (int x = fromX - 1; x >= 0; x--) {
		if (x == toX && fromY == toY) {
			if (isFieldEmpty(board, toX, toY))
				return true;

			if (piecesHaveDifferentColor(board, fromX, fromY, toX, toY))
				return true;

			return false;
		}
		if (!isFieldEmpty(board, x, fromY))
			return false;
	}

	return false;
}

bool Rook::downPossible(chess::Piece* (*board)[8], int fromX, int fromY, int toX, int toY) {
	for (int y = fromY + 1; y < 8; y++) {
		if (fromX == toX && y == toY) {
			if (isFieldEmpty(board, toX, toY))
				return true;

			if (piecesHaveDifferentColor(board, fromX, fromY, toX, toY))
				return true;

			return false;
		}
		if (!isFieldEmpty(board, fromX, y))
			return false;
	}
	return false;
}

chess::Piece* Rook::getDeepCopy() {
	Rook* rook = new Rook(this->color);
	rook->moved = this->moved;

	return rook;
}

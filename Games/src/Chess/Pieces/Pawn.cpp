#include "Chess/Pieces/Pawn.h"

Pawn::Pawn(chess::PieceColor color) 
{
	this->color = color;
	this->pieceChar = 'P';
	this->moved = false;
	this->enPassantDirection = 0;
}

bool Pawn::isMoveValid(chess::Piece* (*board)[8], const chess::Move& move) 
{

	if (normalMovementStrategyPossible(board, move.fromX, move.fromY, move.toX, move.toY))
		return true;
	if (color == chess::PieceColor::BLACK)
		return enPassantPossible(board, move.fromX, move.fromY, move.toX, move.toY, 1);
	else
		return enPassantPossible(board, move.fromX, move.fromY, move.toX, move.toY, -1);
}

void Pawn::makeMove(chess::Piece* (*board)[8], const chess::Move& move) 
{
	if (isEnPassantMove(board, move.fromX, move.fromY, move.toX, move.toY)) {
		board[move.fromX][move.fromY]->moved = true;
		board[move.toX][move.toY] = board[move.fromX][move.fromY];
		board[move.fromX][move.fromY] = nullptr;

		if (board[move.toX][move.fromY] != nullptr)
			delete board[move.toX][move.fromY];
		board[move.toX][move.fromY] = nullptr;

	}
	else {
		setEnPassantDirectionsOfOneColorToZero(board, board[move.fromX][move.fromY]->getPieceColor());
		updateEnPassantDirection(board, move.fromX, move.fromY, move.toX, move.toY);
		movePieceToPosition(board, move);
	}
}

bool Pawn::normalMovementStrategyPossible(chess::Piece* (*board)[8], int fromX, int fromY, int toX, int toY) 
{
	if (board[fromX][fromY]->getPieceColor() == chess::PieceColor::BLACK)
		return normalMovementStrategyForBlackPossible(board, fromX, fromY, toX, toY);
	else
		return normalMovementStrategyForWhitePossible(board, fromX, fromY, toX, toY);
}

bool
Pawn::normalMovementStrategyForBlackPossible(chess::Piece* (*board)[8], int fromX, int fromY, int toX, int toY) 
{
	if (fromX == toX && fromY + 1 == toY && board[toX][toY] == nullptr)
		return true;

	if ((fromY == 1) && fromX == toX && fromY + 2 == toY && isFieldEmpty(board, fromX, fromY + 1)
		&& isFieldEmpty(board, fromX, fromY + 2) && !moved)
		return true;

	if (fromX == toX - 1 || fromX == toX + 1) {
		if (fromY + 1 == toY && !isFieldEmpty(board, toX, toY)
			&& board[fromX][fromY]->getPieceColor() != board[toX][toY]->getPieceColor())
			return true;
	}

	return false;
}

bool Pawn::normalMovementStrategyForWhitePossible(chess::Piece* (*board)[8], int fromX, int fromY, int toX, int toY) 
{
	if (fromX == toX && fromY - 1 == toY && board[toX][toY] == nullptr)
		return true;

	if (fromX == toX && fromY - 2 == toY && isFieldEmpty(board, fromX, fromY - 1)
		&& isFieldEmpty(board, fromX, fromY - 2) && (fromY == 6) && !moved)
		return true;

	if (fromX == toX - 1 || fromX == toX + 1) {
		if (fromY - 1 == toY && !isFieldEmpty(board, toX, toY)
			&& board[fromX][fromY]->getPieceColor() != board[toX][toY]->getPieceColor())
			return true;
	}

	return false;
}

bool
Pawn::enPassantPossible(chess::Piece* (*board)[8], int fromX, int fromY, int toX, int toY, int direction) 
{
	if (enPassantDirection == 0)
		return false;

	if ((toX == fromX + enPassantDirection) && (toY - fromY == direction))
		return true;

	return false;
}

bool
Pawn::isEnPassantMove(chess::Piece* (*board)[8], int fromX, int fromY, int toX, int toY) 
{
	if (abs(fromX - toX) == 1 && abs(fromY - toY) == 1 && isFieldEmpty(board, toX, toY))
		return true;

	return false;
}

void Pawn::updateEnPassantDirection(chess::Piece* (*board)[8], int fromX, int fromY, int toX, int toY) 
{
	if (abs(fromY - toY) != 2)
		return;

	int left = toX - 1;
	int right = toX + 1;

	if (left >= 0)
		updateEnPassant(board, fromX, fromY, left, toY);

	if (right < 8)
		updateEnPassant(board, fromX, fromY, right, toY);
}

void Pawn::updateEnPassant(chess::Piece* (*board)[8], int fromX, int fromY, int updatePawnX, int updatePawnY) 
{
	if (isFieldEmpty(board, updatePawnX, updatePawnY))
		return;

	if (!piecesHaveDifferentColor(board, fromX, fromY, updatePawnX, updatePawnY))
		return;


	if (board[updatePawnX][updatePawnY]->getPieceChar() != 'P')
		return;

	Pawn* pawn = dynamic_cast<Pawn*>(board[updatePawnX][updatePawnY]);
	pawn->enPassantDirection = fromX - updatePawnX;
}

chess::Piece* Pawn::getDeepCopy() 
{
	Pawn* pawn = new Pawn(this->color);
	pawn->moved = this->moved;
	pawn->enPassantDirection = this->enPassantDirection;

	return pawn;
}

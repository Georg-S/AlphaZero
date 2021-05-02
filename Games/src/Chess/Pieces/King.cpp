#include "Chess/Pieces/King.h"

King::King(chess::PieceColor color)
{
	this->color = color;
	this->pieceChar = 'K';
	this->moved = false;
}

bool King::isMoveValid(chess::Piece* board[8][8], const chess::Move& move)
{

	if (normalMovePossible(board, move.fromX, move.fromY, move.toX, move.toY))
		return true;
	if (castlingPossible(board, move.fromX, move.fromY, move.toX, move.toY))
		return true;
	return false;
}

void King::makeMove(chess::Piece* board[8][8], const chess::Move& move)
{
	setEnPassantDirectionsOfOneColorToZero(board, board[move.fromX][move.fromY]->getPieceColor());

	if (isCastlingMove(move.fromX, move.toX))
		makeCastlingMove(board, move.fromX, move.fromY, move.toX, move.toY);
	else
		movePieceToPosition(board, move);
}

bool King::normalMovePossible(chess::Piece* board[8][8], int fromX, int fromY, int toX,
	int toY)
{

	if (fromX == toX && fromY == toY)
		return false;

	if (abs(fromX - toX) > 1 || abs(fromY - toY) > 1)
		return false;

	if (isFieldEmpty(board, toX, toY))
		return true;

	if (piecesHaveDifferentColor(board, fromX, fromY, toX, toY))
		return true;

	return false;
}

bool
King::castlingPossible(chess::Piece* board[8][8], int fromX, int fromY, int toX, int toY)
{
	if (moved || fromY != toY)
		return false;
	if (abs(fromX - toX) != 2)
		return false;

	int rookX = -1;
	int buf = fromX - toX;
	if (buf > 0)
		rookX = 0;
	else
		rookX = 7;

	if (!isValidRookForCastlingField(board, rookX, fromY))
		return false;

	if (piecesHaveDifferentColor(board, fromX, fromY, rookX, fromY))
		return false;

	int direction = (toX - fromX) / abs(toX - fromX);

	for (int x = fromX + direction; x != rookX; x += direction)
	{
		if (!(isFieldEmpty(board, x, fromY)))
		{
			return false;
		}
	}

	// toX = 0   fromX = 4
	// direction = 0-4 / 4 = -1
	// x = 4 - 1
	// x = 3
	if (isCheck(board, fromX, fromY))
		return false;

	if (testCheckOnPositionAndResetBoard(board, fromX, fromY, fromX + direction, toY)
		|| testCheckOnPositionAndResetBoard(board, fromX, fromY, fromX + (2 * direction), toY))
		return false;

	return true;
}

bool King::isValidRookForCastlingField(chess::Piece* board[8][8], int rookX, int rookY)
{
	if (isFieldEmpty(board, rookX, rookY))
		return false;

	if (board[rookX][rookY]->moved)
		return false;

	if (board[rookX][rookY]->getPieceChar() != 'R')
		return false;

	return true;
}

bool King::testCheckOnPositionAndResetBoard(chess::Piece* board[8][8], int fromX, int fromY, int toX,
	int toY)
{
	if (board[toX][toY] != nullptr)
	{
		debugPrintBoardToConsole(board);
		std::cout << "We have a Problem " << std::endl;
	}


	board[toX][toY] = board[fromX][fromY];
	board[fromX][fromY] = nullptr;

	bool check = isCheck(board, toX, toY);
	board[fromX][fromY] = board[toX][toY];
	board[toX][toY] = nullptr;

	return check;
}

bool King::isCastlingMove(int fromX, int toX)
{
	return (abs(fromX - toX) == 2);
}

void King::makeCastlingMove(chess::Piece* board[8][8], int fromX, int fromY, int toX, int toY)
{
	board[fromX][fromY]->moved = true;
	int direction = (toX - fromX) / abs(toX - fromX);
	int rookX;

	board[toX][toY] = board[fromX][fromY];
	board[fromX][fromY] = nullptr;

	if (direction < 0)
		rookX = 0;
	else
		rookX = 7;

	if (!isValidRookForCastlingField(board, rookX, fromY))
		return;

	board[rookX][fromY]->moved = true;
	board[toX - direction][toY] = board[rookX][fromY];
	board[rookX][fromY] = nullptr;
}

chess::Piece* King::getDeepCopy()
{
	King* king = new King(this->color);
	king->moved = this->moved;

	return king;
}

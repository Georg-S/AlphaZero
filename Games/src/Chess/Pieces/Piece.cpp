#include "Chess/Pieces/Piece.h"

chess::PieceColor chess::Piece::getPieceColor()
{
	return color;
}

char chess::Piece::getPieceChar()
{
	return pieceChar;
}

bool chess::Piece::isFieldEmpty(chess::Piece* (*board)[8], int x, int y)
{
	return board[x][y] == nullptr;
}

bool chess::Piece::piecesHaveDifferentColor(chess::Piece* (*board)[8], int fromX, int fromY, int toX, int toY)
{
	if (board[fromX][fromY] == nullptr || board[toX][toY] == nullptr)
		return false;

	return board[fromX][fromY]->getPieceColor() != board[toX][toY]->getPieceColor();
}

void chess::Piece::movePieceToPosition(chess::Piece* (*board)[8], const chess::Move& move)
{
	Piece* buf = board[move.fromX][move.fromY];
	buf->moved = true;
	board[move.fromX][move.fromY] = nullptr;

	if (board[move.toX][move.toY] != nullptr)
		delete board[move.toX][move.toY];

	board[move.toX][move.toY] = buf;
}

void chess::Piece::setEnPassantDirectionsOfOneColorToZero(chess::Piece* (*board)[8], chess::PieceColor color)
{
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			if (isFieldEmpty(board, x, y))
				continue;
			if (board[x][y]->getPieceChar() != 'P')
				continue;
			if (board[x][y]->getPieceColor() != color)
				continue;

			board[x][y]->enPassantDirection = 0;
		}
	}
}

bool chess::Piece::isCheck(chess::Piece* (*board)[8], const chess::PieceColor& color)
{
	int kingX = -1;
	int kingY = -1;

	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			if (isFieldEmpty(board, x, y))
				continue;
			if (board[x][y]->getPieceColor() != color)
				continue;

			if (board[x][y]->getPieceChar() == 'K')
			{
				kingX = x;
				kingY = y;
			}
		}
	}

	return isCheck(board, kingX, kingY);
}

bool chess::Piece::isCheck(chess::Piece* (*board)[8], int kingX, int kingY)
{
	if (isFieldEmpty(board, kingX, kingY))
		return false;

	chess::PieceColor kingColor = board[kingX][kingY]->getPieceColor();
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			if (isFieldEmpty(board, x, y))
				continue;
			if (board[x][y]->getPieceColor() == kingColor)
				continue;

			if (board[x][y]->isMoveValid(board, Move(x, y, kingX, kingY)))
				return true;
		}
	}

	return false;
}

void chess::Piece::debugPrintBoardToConsole(chess::Piece* (*board)[8])
{
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			if (board[x][y] == nullptr)
				std::cout << "-" << "  ";
			else
				std::cout << board[x][y]->getPieceChar() << "  ";
		}
		std::cout << std::endl;
	}
}

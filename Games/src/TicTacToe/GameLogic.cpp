#include "TicTacToe/GameLogic.h"

using namespace ttt;
using namespace game;

Board::Board()
	: m_crosses(0),
	m_dots(0),
	m_occupied(0)
{}

ttt::Board::Board(const std::string& str)
{
	assert(str.size() == 9);
	m_crosses = 0;
	m_dots = 0;

	for (int i = 0; i < 9; i++) 
	{
		if (str.at(i) == 'X')
			setBit(m_crosses, i);
		else if (str.at(i) == 'O')
			setBit(m_dots, i);
	}
	m_occupied = m_dots | m_crosses;
}

void ttt::Board::makeMove(int pos, PlayerColor color)
{
	assert(color != PlayerColor::NONE);

	if (color == PlayerColor::CROSS)
		game::setBit(m_crosses, pos);
	else
		game::setBit(m_dots, pos);

	m_occupied = m_crosses | m_dots;
}

void ttt::Board::makeMove(const Move& move, PlayerColor color)
{
	makeMove(move.y * 3 + move.x, color);
}

bool ttt::Board::isBoardFull() const
{
	return m_occupied == fullBoard;
}

bool ttt::Board::isMovePossible(const Move& move) const
{
	return !isBitSet(m_occupied, move.x + move.y * 3);
}

PlayerColor ttt::Board::at(int x, int y) const
{
	int index = x + y * 3;
	if (!isBitSet(m_occupied, index))
		return PlayerColor::NONE;
	if (isBitSet(m_crosses, index))
		return PlayerColor::CROSS;

	return PlayerColor::DOT;
}

uint32_t ttt::Board::getPieces(PlayerColor color) const
{
	assert(color != PlayerColor::NONE);

	if (color == PlayerColor::DOT)
		return this->m_dots;
	else
		return this->m_crosses;
}

uint32_t ttt::Board::occupied() const
{
	return m_occupied;
}

std::string ttt::Board::toString() const
{
	std::string result;
	for(int i = 0; i < 9; i++)
	{
		char c = '-';
		if (isBitSet(m_crosses, i))
			c = 'X';
		else if (isBitSet(m_dots, i))
			c = 'O';

		result += c;
	}

	return result;
}

bool ttt::isGameOver(const Board& board)
{
	return board.isBoardFull() || playerWon(board, PlayerColor::CROSS) || playerWon(board, PlayerColor::DOT);
}

bool ttt::playerWon(const Board& board, PlayerColor color)
{
	auto pieces = board.getPieces(color);

	return horizontalThreeInARow(pieces) || verticalThreeInARow(pieces) || diagonalThreeInARow(pieces);
}

bool ttt::horizontalThreeInARow(uint32_t pieces)
{
	constexpr uint32_t firstColumnMask = 438;	// first column is zero
	constexpr uint32_t firstAndSecondColumnMask = 292; // first and second column are zero

	return pieces & ((pieces << 1) & firstColumnMask) & ((pieces << 2) & firstAndSecondColumnMask);
}

bool ttt::verticalThreeInARow(uint32_t pieces)
{
	return pieces & (pieces << 3) & (pieces << 6);
}

bool ttt::diagonalThreeInARow(uint32_t pieces)
{
	constexpr uint32_t diagonalDownMask = 273;
	constexpr uint32_t diagonalUpMask = 84;

	return ((pieces & diagonalDownMask) == diagonalDownMask) || ((pieces & diagonalUpMask) == diagonalUpMask);
}

PlayerColor ttt::getNextPlayer(PlayerColor color)
{
	assert(color != PlayerColor::NONE);

	if (color == PlayerColor::CROSS)
		return PlayerColor::DOT;
	else
		return PlayerColor::CROSS;
}

int ttt::getNextPlayer(int player)
{
	return static_cast<int>(getNextPlayer(static_cast<PlayerColor>(player)));
}

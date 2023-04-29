#include "ConnectFour/GameLogic.h"

using namespace game;

cn4::Board::Board(const std::string& str)
{
	constexpr char yellowPiece = '1';
	constexpr char redPiece = '2';

	assert(str.size() == (boardHeight * boardWidth));
	for (int i = 0; i < (boardHeight * boardWidth); i++)
	{
		char c = str.at(i);
		int row = i / boardWidth;
		int column = i - (boardWidth * row);

		if (c == yellowPiece)
		{
			setBit(m_yellowPieces, i);
			m_occupiedColumnSize[column] = std::max(m_occupiedColumnSize[column], row + 1);
		}
		else if (c == redPiece)
		{
			setBit(m_redPieces, i);
			m_occupiedColumnSize[column] = std::max(m_occupiedColumnSize[column], row + 1);
		}
	}
	m_occupied = m_redPieces | m_yellowPieces;
}

bool cn4::Board::isFull() const
{
	return m_occupied == fullBoardMask;
}

uint64_t cn4::Board::occupied() const
{
	return m_occupied;
}

uint64_t cn4::Board::yellowPieces() const
{
	return m_yellowPieces;
}

uint64_t cn4::Board::redPieces() const
{
	return m_redPieces;
}

void cn4::Board::makeMove(int column, PlayerColor color)
{
	assert(m_occupiedColumnSize[column] < boardHeight);
	assert(color != PlayerColor::NONE);
	if (color == PlayerColor::RED)
		setBit(m_redPieces, column + (boardWidth * m_occupiedColumnSize[column]));
	else
		setBit(m_yellowPieces, column + (boardWidth * m_occupiedColumnSize[column]));

	m_occupiedColumnSize[column]++;
	m_occupied = m_redPieces | m_yellowPieces;
}

cn4::PlayerColor cn4::Board::at(int column, int row) const
{
	int bitIndex = column + (row * boardWidth);
	if (isBitSet(m_redPieces, bitIndex))
		return PlayerColor::RED;
	if (isBitSet(m_yellowPieces, bitIndex))
		return PlayerColor::YELLOW;
	return PlayerColor::NONE;
}

static void setPieces(std::string& str, uint64_t pieces, char pieceC)
{
	while (pieces)
	{
		int index = lowestBitIndex(pieces);
		str[index] = pieceC;
		resetLowestBit(pieces);
	}
}

std::string cn4::Board::toString() const
{
	std::string str = "000000000000000000000000000000000000000000"; // Empty board string
	setPieces(str, m_yellowPieces, '1');
	setPieces(str, m_redPieces, '2');

	return str;
}

bool cn4::operator<(const Board& lhs, const Board& rhs)
{
	if (lhs.m_yellowPieces == rhs.m_yellowPieces)
		return lhs.m_redPieces < rhs.m_redPieces;

	return lhs.m_yellowPieces < rhs.m_yellowPieces;
}

cn4::PlayerColor cn4::getPlayerWon(const Board& board)
{
	if (hasPlayerWon(board.redPieces()))
		return PlayerColor::RED;
	if (hasPlayerWon(board.yellowPieces()))
		return PlayerColor::YELLOW;

	return PlayerColor::NONE;
}

bool cn4::isGameOver(const Board& board)
{
	return board.isFull() || hasPlayerWon(board.yellowPieces()) || hasPlayerWon(board.redPieces());
}

cn4::PlayerColor cn4::getNextPlayer(PlayerColor color)
{
	assert(color != PlayerColor::NONE);
	if (color == PlayerColor::RED)
		return PlayerColor::YELLOW;
	else
		return PlayerColor::RED;
}

int cn4::getNextPlayer(int playerColor)
{
	return static_cast<int>(getNextPlayer(PlayerColor(playerColor)));
}

bool cn4::hasPlayerWon(uint64_t playerPieces)
{
	return verticalFourInARow(playerPieces) || horizontalFourInARow(playerPieces) || diagonalFourInARow(playerPieces);
}

static bool fourInARow(uint64_t pieces, int oneShiftVale, uint64_t maskAfterFirstShift)
{
	auto buf = pieces & (pieces << 2 * oneShiftVale) & maskAfterFirstShift;

	return  buf & (buf << oneShiftVale);
}

bool cn4::verticalFourInARow(uint64_t pieces)
{
	constexpr int shiftOneRow = boardWidth;
	return fourInARow(pieces, shiftOneRow, fullBoardMask);
}

bool cn4::horizontalFourInARow(uint64_t pieces)
{
	return fourInARow(pieces, 1, leftTwoColumnsZeroMask);
}

bool cn4::diagonalFourInARow(uint64_t pieces)
{
	return diagonalDownFourInARow(pieces) || diagonalUpFourInARow(pieces);
}

bool cn4::diagonalDownFourInARow(uint64_t pieces)
{
	constexpr int shiftDiagonalUpOneTime = boardWidth - 1;
	return fourInARow(pieces, shiftDiagonalUpOneTime, rightTwoColumnsZeroMask);
}

bool cn4::diagonalUpFourInARow(uint64_t pieces)
{
	constexpr int shiftDiagonalDownOneTime = boardWidth + 1;
	return fourInARow(pieces, shiftDiagonalDownOneTime, leftTwoColumnsZeroMask);
}

bool cn4::isMovePossible(const Board& board, int move)
{
	constexpr int uppermostRowBitShift = 35;
	return !game::isBitSet(board.occupied(), uppermostRowBitShift + move);
}

std::vector<int> cn4::getAllPossibleMoves(const Board& board)
{
	std::vector<int> result;
	result.reserve(7);

	for (int i = 0; i < boardWidth; i++)
	{
		if (isMovePossible(board, i))
			result.push_back(i);
	}

	return result;
}

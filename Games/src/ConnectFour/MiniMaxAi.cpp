#include "ConnectFour/MiniMaxAi.h"

using namespace cn4;

cn4::MiniMaxAi::MiniMaxAi(int depth)
	: m_depth(depth)
	, m_aiColor(PlayerColor::NONE)
{
}

int cn4::MiniMaxAi::getMove(const std::string& state, int playerColor)
{
	m_aiColor = PlayerColor(playerColor);
	Board board = Board(state);
	std::vector<int> possibleMoves = getAllPossibleMoves(board);
	orderMoves(possibleMoves);

	std::vector<int> values;

	for (auto move : possibleMoves)
	{
		Board copyBoard = board;
		copyBoard.makeMove(move, m_aiColor);
		int value = evaluateBoard(copyBoard, m_depth, getNextPlayer(m_aiColor), false, INT_MIN, INT_MAX);
		values.push_back(value);
	}
	std::vector<int> bestMoves = getBestMoves(possibleMoves, values);

	return game::getRandomElement(bestMoves);
}

int cn4::MiniMaxAi::evaluateBoard(const cn4::Board& board, int depth, PlayerColor currentPlayer, bool maximizingPlayer, int alpha, int beta)
{
	PlayerColor nextPlayer = getNextPlayer(currentPlayer);
	PlayerColor playerWon = getPlayerWon(board);

	if (playerWon == m_aiColor)
		return m_maxValue + depth;
	if (playerWon != PlayerColor::NONE)
		return m_minValue + depth;
	if (board.isFull())
		return 0;
	if (depth == 0)
		return staticBoardEvaluation(board);

	int boardValue = 0;
	if (maximizingPlayer)
		boardValue = m_minValue;
	else
		boardValue = m_maxValue;


	std::vector<int> possibleMoves = getAllPossibleMoves(board);
	orderMoves(possibleMoves);

	for (auto move : possibleMoves)
	{
		cn4::Board copyBoard = Board(board);
		copyBoard.makeMove(move, currentPlayer);
		int moveValue = evaluateBoard(copyBoard, depth - 1, nextPlayer, !maximizingPlayer, alpha, beta);

		if (maximizingPlayer)
		{
			boardValue = std::max(moveValue, boardValue);
			alpha = std::max(boardValue, alpha);
			if (beta <= alpha)
				break;
		}
		else
		{
			boardValue = std::min(moveValue, boardValue);
			beta = std::min(boardValue, beta);
			if (beta <= alpha)
				break;
		}
	}
	return boardValue;
}

void cn4::MiniMaxAi::orderMoves(std::vector<int>& moves) const
{
	constexpr int middle = 3;
	std::sort(moves.begin(), moves.end(), [middle](int a, int b) { return abs(middle - a) < abs(middle - b); });
}

static int getCountOfPossibleConnectFours(uint64_t pieces, int count_connected, int shiftValue, uint64_t occupied, uint64_t additionalMask)
{
	int connectedShifts = count_connected - 1;
	int occupationShifts = 4 - count_connected;

	auto buf = pieces;
	while (connectedShifts--)
	{
		buf &= pieces & (pieces << shiftValue);
		buf &= (buf << shiftValue);
		buf &= additionalMask;
	}

	const uint64_t invertedOccupied = ~occupied;
	while (occupationShifts--)
	{
		buf = (buf << shiftValue) & invertedOccupied;
		buf &= additionalMask;
	}

	int count = 0;
	while (buf)
	{
		game::resetLowestBit(buf);
		count++;
	}

	return count;
}

static int getScore(const cn4::Board& board, int shift, uint64_t additionalMask)
{
	constexpr int threeConectedWeight = 3;
	constexpr int twoconnectedWeight = 2;
	int yellowScoreThreeConnected = getCountOfPossibleConnectFours(board.yellowPieces(), 3, shift, board.occupied(), additionalMask);
	int redScoreThreeConnected = getCountOfPossibleConnectFours(board.redPieces(), 3, shift, board.occupied(), additionalMask);
	int yellowScoreTwoConnected = getCountOfPossibleConnectFours(board.yellowPieces(), 2, shift, board.occupied(), additionalMask);
	int redScoreTwoConnected = getCountOfPossibleConnectFours(board.redPieces(), 2, shift, board.occupied(), additionalMask);

	int yellowScore = (threeConectedWeight * yellowScoreThreeConnected) + (twoconnectedWeight * yellowScoreTwoConnected);
	int redScore = (threeConectedWeight * redScoreThreeConnected) + (twoconnectedWeight * redScoreTwoConnected);

	return yellowScore - redScore;
}

int cn4::MiniMaxAi::staticBoardEvaluation(const cn4::Board& board)
{
	int horizontalScore = getScore(board, 1, leftColumnZeroMask);
	int verticalScore = getScore(board, boardWidth, fullBoardMask); // fullBoardMask = Does nothing
	int diagonalUpScore = getScore(board, boardWidth + 1, leftColumnZeroMask);
	int diagonalDownScore = getScore(board, boardWidth - 1, rightColumnZeroMask);

	int score = horizontalScore + verticalScore + diagonalUpScore + diagonalDownScore;
	if (m_aiColor == PlayerColor::RED)
		score = -score;

	return score;
}

std::vector<int> cn4::MiniMaxAi::getBestMoves(std::vector<int>& moves, std::vector<int>& values)
{
	assert(!moves.empty());
	assert(moves.size() == values.size());
	const int highestValue = *(std::max_element(values.begin(), values.end()));

	std::vector <int> bestMoves;
	for (int i = 0; i < moves.size(); i++)
	{
		if (values[i] == highestValue)
			bestMoves.push_back(moves[i]);
	}

	return bestMoves;
}

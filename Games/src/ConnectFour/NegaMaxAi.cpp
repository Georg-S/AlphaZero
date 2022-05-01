#include "ConnectFour/NegaMaxAi.h"

using namespace cn4;

cn4::NegaMaxAi::NegaMaxAi(int depth)
	: m_depth(depth)
{
}

int cn4::NegaMaxAi::getMove(const std::string& state, int playerColor)
{
	Board board = Board(state);
	std::vector<int> possibleMoves = getAllPossibleMoves(board);
	orderMoves(possibleMoves);

	std::vector<int> values;
	PlayerColor aiColor = PlayerColor(playerColor);

	for (auto move : possibleMoves)
	{
		Board copyBoard = board;
		copyBoard.makeMove(move, aiColor);
		int value = negamax(copyBoard, m_depth, getNextPlayer(aiColor), m_minValue, m_maxValue);
		values.push_back(value);
	}
	std::vector<int> bestMoves = game::getHighestValueElements(possibleMoves, values);

	return game::getRandomElement(bestMoves);
}

int cn4::NegaMaxAi::negamax(const Board& board, int depth, PlayerColor currentPlayer, int alpha, int beta)
{
	PlayerColor nextPlayer = getNextPlayer(currentPlayer);
	PlayerColor playerWon = getPlayerWon(board);

	if (playerWon != PlayerColor::NONE)
		return -(m_loseValue - depth);
	if (board.isFull())
		return 0;
	if (depth == 0)
		return -staticBoardEvaluation(board, currentPlayer);

	int boardValue = m_minValue;

	std::vector<int> possibleMoves = getAllPossibleMoves(board);
	orderMoves(possibleMoves);

	for (auto move : possibleMoves)
	{
		Board copyBoard = board;
		copyBoard.makeMove(move, currentPlayer);
		int moveValue = negamax(copyBoard, depth - 1, nextPlayer, -beta, -alpha);
		boardValue = std::max(moveValue, boardValue);
		alpha = std::max(alpha, boardValue);

		if (alpha >= beta)
			break;
	}
	return -boardValue;
}

void cn4::NegaMaxAi::orderMoves(std::vector<int>& moves) const
{
	constexpr int middle = 3;
	std::sort(moves.begin(), moves.end(), [middle](int a, int b)
	{
		return abs(middle - a) < abs(middle - b);
	});
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

static int getScore(const Board& board, int shift, uint64_t additionalMask)
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

int cn4::NegaMaxAi::staticBoardEvaluation(const cn4::Board& board, PlayerColor currentPlayer) const
{
	int horizontalScore = getScore(board, 1, leftColumnZeroMask);
	int verticalScore = getScore(board, boardWidth, fullBoardMask); // fullBoardMask = Does nothing
	int diagonalUpScore = getScore(board, boardWidth + 1, leftColumnZeroMask);
	int diagonalDownScore = getScore(board, boardWidth - 1, rightColumnZeroMask);

	int score = horizontalScore + verticalScore + diagonalUpScore + diagonalDownScore;
	if (currentPlayer == PlayerColor::RED)
		score = -score;

	return score;
}

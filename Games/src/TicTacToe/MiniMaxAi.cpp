#include "TicTacToe/MiniMaxAi.h"

using namespace ttt;

int ttt::MiniMaxAi::getMove(const std::string& state, int color)
{
	Board board = Board(state);
	Move move = getMove(board, color);

	return move.x + move.y * 3;
}

Move ttt::MiniMaxAi::getMove(Board board, int color)
{
	std::vector<int> values;
	m_color = static_cast<PlayerColor>(color);
	m_opponentColor = getNextPlayer(m_color);

	std::vector<Move> possibleMoves = getAllPossibleMoves<Move>(board);
	for (const auto& move : possibleMoves) 
	{
		Board copyBoard = board;
		copyBoard.makeMove(move, m_color);
		values.push_back(evaluateBoard(copyBoard, m_opponentColor, false, INT_MIN, INT_MAX));
	}

	std::vector<Move> bestMoves = getBestMoves(possibleMoves, values);

	return game::getRandomElement(bestMoves);
}

int ttt::MiniMaxAi::evaluateBoard(const Board& board, PlayerColor currentPlayer, bool maximizingPlayer, int alpha, int beta)
{
	PlayerColor nextPlayer = getNextPlayer(currentPlayer);
	if (playerWon(board, m_color))
		return 1;
	else if (playerWon(board, m_opponentColor))
		return -1;
	else if (board.isBoardFull())
		return 0;

	int boardValue = maximizingPlayer ? -10 : 10;
	std::vector<Move> allPossibleMoves = getAllPossibleMoves<Move>(board);

	for (const auto& move : allPossibleMoves) 
	{
		Board copyBoard = board;
		copyBoard.makeMove(move, currentPlayer);
		const int moveValue = evaluateBoard(copyBoard, nextPlayer, !maximizingPlayer, alpha, beta);

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

std::vector<Move> ttt::MiniMaxAi::getBestMoves(const std::vector<Move>& moves, const std::vector<int>& values)
{
	assert(moves.size() == values.size());
	const int highestValue = *(std::max_element(values.begin(), values.end()));

	std::vector <Move> bestMoves;
	for (int i = 0; i < moves.size(); i++)
	{
		if (values[i] == highestValue)
			bestMoves.push_back(moves[i]);
	}

	return bestMoves;
}

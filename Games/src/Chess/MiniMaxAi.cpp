#include "Chess/MiniMaxAi.h"

chess::MiniMaxAi::MiniMaxAi(int depth)
{
	this->depth = depth;
}

int chess::MiniMaxAi::getMove(std::string state, int playerColor)
{
	this->aiColor = (chess::PieceColor)playerColor;
	this->otherPlayer = chess::GameLogic::getNextPlayer(aiColor);

	Board board = ReducedChessAdapter::convertStateStringToBoard(state);

	std::vector<chess::Move> possibleMoves = getAllPossibleMoves(board, (PieceColor)playerColor);
	std::vector<int> values(possibleMoves.size(), -1);

	for (int i = 0; i < possibleMoves.size(); i++)
	{
		Board copyBoard = Board(board);
		makeMoveWithPromotion(copyBoard, possibleMoves[i]);
		values[i] = evaluateBoard(copyBoard, this->depth, otherPlayer, false, INT_MIN, INT_MAX);
		copyBoard.deletePieces();
	}
	board.deletePieces();

	std::vector<chess::Move> bestMoves = getBestMoves(possibleMoves, values);

	return getRandomMove(bestMoves).getSingleIntRepresentation();
}

std::vector<chess::Move> chess::MiniMaxAi::getAllPossibleMoves(chess::Board& board, const chess::PieceColor& color)
{
	std::vector<chess::Move> possibleMoves;

	for (int fromX = 0; fromX < 8; fromX++)
	{
		for (int fromY = 0; fromY < 8; fromY++)
		{
			if (board.board[fromX][fromY] == nullptr)
				continue;
			if (board.board[fromX][fromY]->getPieceColor() != color)
				continue;
			getAllPossibleMovesForPiece(board, fromX, fromY, possibleMoves);
		}
	}

	return possibleMoves;
}

void chess::MiniMaxAi::getAllPossibleMovesForPiece(chess::Board& board, int fromX, int fromY,
	std::vector<chess::Move>& moves)
{
	for (int toX = 0; toX < 8; toX++)
	{
		for (int toY = 0; toY < 8; toY++)
		{
			chess::Move move = chess::Move(fromX, fromY, toX, toY);
			if (chess::GameLogic::isMoveValid(board, Move(fromX, fromY, toX, toY)))
				moves.push_back(move);
		}
	}
}

int chess::MiniMaxAi::evaluateBoard(chess::Board& board, int depth, chess::PieceColor currentPlayer,
	bool maximizingPlayer, int alpha, int beta)
{
	if (chess::GameLogic::isCheckMate(board, aiColor))
		return (-checkMateValue - depth);
	if (chess::GameLogic::isCheckMate(board, otherPlayer))
		return checkMateValue + depth;
	if (chess::GameLogic::isStaleMate(board, aiColor))
		return 0;
	if (chess::GameLogic::isStaleMate(board, otherPlayer))
		return 0;
	if (depth == 0)
		return staticEvaluateBoard(board);


	int boardValue = 0;
	if (maximizingPlayer)
		boardValue = minValue;
	else
		boardValue = maxValue;

	std::vector<chess::Move> possibleMoves = getAllPossibleMoves(board, currentPlayer);
	for (int i = 0; i < possibleMoves.size(); i++)
	{
		chess::Board copyBoard = Board(board);
		makeMoveWithPromotion(copyBoard, possibleMoves[i]);

		int moveValue = evaluateBoard(copyBoard, depth - 1, chess::GameLogic::getNextPlayer(currentPlayer), !maximizingPlayer, alpha, beta);
		copyBoard.deletePieces();

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

int chess::MiniMaxAi::staticEvaluateBoard(chess::Board& board)
{
	int boardValue = 0;
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			if (board.board[x][y] == nullptr)
				continue;
			boardValue += getPieceValue(board.board[x][y]);
		}
	}

	return boardValue;
}

void chess::MiniMaxAi::makeMoveWithPromotion(chess::Board& board, const chess::Move& move)
{
	chess::GameLogic::makeMove(board, move);

	if (chess::GameLogic::pawnReachedEndOfBoard(board))
	{
		replacePawnsInRowWithQueens(board, 0);
		replacePawnsInRowWithQueens(board, 7);
	}
}

void chess::MiniMaxAi::replacePawnsInRowWithQueens(chess::Board& board, int row)
{
	for (int x = 0; x < 8; x++)
	{
		if (board.board[x][row] == nullptr)
			continue;
		if (board.board[x][row]->getPieceChar() != 'P')
			continue;

		chess::PieceColor color = board.board[x][row]->getPieceColor();
		bool moved = board.board[x][row]->moved;
		delete board.board[x][row];
		board.board[x][row] = new Queen(color);
		board.board[x][row]->moved = moved;
	}
}

int chess::MiniMaxAi::getPieceValue(chess::Piece* piece)
{
	int value = 0;

	switch (piece->getPieceChar())
	{
	case 'P':
		value = 1;
		break;
	case 'B':
		value = 3;
		break;
	case 'H':
		value = 3;
		break;
	case 'R':
		value = 5;
		break;
	case 'Q':
		value = 9;
		break;
	case 'K':
		value = 0;
		break;
	default:
		value = 0;
		break;
	}

	if (piece->getPieceColor() == otherPlayer)
		value *= -1;

	return value;
}

std::vector<chess::Move> chess::MiniMaxAi::getBestMoves(std::vector<chess::Move>& moves, std::vector<int>& values)
{
	std::vector <chess::Move> bestMoves;
	int highestValue = *(std::max_element(values.begin(), values.end()));

	for (int i = 0; i < moves.size(); i++)
	{
		if (values[i] == highestValue)
			bestMoves.push_back(moves[i]);
	}

	return bestMoves;
}

chess::Move chess::MiniMaxAi::getRandomMove(const std::vector<chess::Move>& moves)
{
	int randIndex = rand() % moves.size();
	return moves[randIndex];
}

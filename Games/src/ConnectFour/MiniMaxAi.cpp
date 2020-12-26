#include "ConnectFour/MiniMaxAi.h"


cn4::MiniMaxAi::MiniMaxAi(int depth)
{
	this->depth = depth;
	ConnectFourAdapter adap = ConnectFourAdapter();
}


int cn4::MiniMaxAi::getMove(std::string state, int playerColor)
{
	aiColor = playerColor;

	cn4::Board board = adap.convertStringToBoard(state);
	std::vector<int> possibleMoves;
	getAllPossibleMoves(possibleMoves, board);
	orderMoves(possibleMoves);

	std::vector<int> values;

	for (int i = 0; i < possibleMoves.size(); i++) {
		cn4::Board copyBoard = cn4::Board(board);
		makeMove(copyBoard, possibleMoves[i], playerColor);

		int value = evaluateBoard(copyBoard, depth, adap.getNextPlayer(playerColor), false, INT_MIN, INT_MAX);
		values.push_back(value);
	}
	std::vector<int> bestMoves = getBestMoves(possibleMoves, values);

	return getRandomMove(bestMoves);
}

void cn4::MiniMaxAi::makeMove(cn4::Board& board, int move, int currenPlayer)
{
	for (int i = 0; i < board.height; i++) {
		if (board.board[move][i] == 0) {
			board.board[move][i] = currenPlayer;
			return;
		}
	}
}

int cn4::MiniMaxAi::evaluateBoard(const cn4::Board& board, int depth, int currentPlayer, bool maximizingPlayer, int alpha,
	int beta)
{

	int nextPlayer = adap.getNextPlayer(currentPlayer);

	if (cn4::GameLogic::isDraw(board))
		return 0;
	else if (cn4::GameLogic::getPlayerWon(board) == (cn4::PlayerColor)aiColor)
		return maxValue + depth;
	else if (cn4::GameLogic::getPlayerWon(board) != cn4::PlayerColor::NONE)
		return minValue + depth;
	else if (depth == 0)
		return staticBoardEvaluation(board);

	int boardValue = 0;
	if (maximizingPlayer)
		boardValue = minValue;
	else
		boardValue = maxValue;


	std::vector<int> possibleMoves;
	getAllPossibleMoves(possibleMoves, board);
	orderMoves(possibleMoves);

	for (int i = 0; i < possibleMoves.size(); i++) {
		cn4::Board copyBoard = Board(board);
		makeMove(copyBoard, possibleMoves[i], currentPlayer);
		int moveValue = evaluateBoard(copyBoard, depth - 1, nextPlayer, !maximizingPlayer, alpha, beta);

		if (maximizingPlayer) {
			boardValue = std::max(moveValue, boardValue);
			alpha = std::max(boardValue, alpha);
			if (beta <= alpha)
				break;
		}
		else {
			boardValue = std::min(moveValue, boardValue);
			beta = std::min(boardValue, beta);
			if (beta <= alpha)
				break;
		}
	}
	return boardValue;
}

void cn4::MiniMaxAi::getAllPossibleMoves(std::vector<int>& destination, const cn4::Board& board) const
{
	for (int i = 0; i < board.width; i++)
		if (board.board[i][board.height - 1] == (int)cn4::PlayerColor::NONE)
			destination.push_back(i);
}

void cn4::MiniMaxAi::orderMoves(std::vector<int>& moves) const
{
	constexpr int middle = 3;
	std::sort(moves.begin(), moves.end(), [middle](int a, int b) {return abs(middle - a) < abs(middle - b); });
}

int cn4::MiniMaxAi::staticBoardEvaluation(const cn4::Board& board)
{
	int boardValue = 0;
	for (int x = 0; x < board.width; x++) {
		for (int y = 0; y < board.height; y++) {
			boardValue += evaluateBoardPosition(board, x, y);
		}
	}

	return boardValue;
}

int cn4::MiniMaxAi::evaluateBoardPosition(const cn4::Board& board, int x, int y)
{
	int value = rightDiagonalUpValue(board, x, y) + rightDiagonalDownValue(board, x, y)
		+ rightHorizontalValue(board, x, y) + upVerticalValue(board, x, y);
	if (board.board[x][y] != aiColor)
		return -value;

	return value;
}

int cn4::MiniMaxAi::rightHorizontalValue(const cn4::Board& board, int x, int y)
{
	if (x + 3 >= board.width)
		return 0;

	int value = 0;
	int color = board.board[x][y];
	for (int i = 1; i < 4; i++) {
		if (board.board[x + i][y] == (int)cn4::PlayerColor::NONE)
			continue;
		if (board.board[x + i][y] != color)
			return 0;

		value++;
	}

	return value;
}

int cn4::MiniMaxAi::rightDiagonalDownValue(const cn4::Board& board, int x, int y)
{
	if (x + 3 >= board.width)
		return 0;
	if (y - 3 < 0)
		return 0;

	int value = 0;
	int color = board.board[x][y];

	for (int i = 1; i < 4; i++) {
		if (board.board[x + i][y - i] == (int)cn4::PlayerColor::NONE)
			continue;
		if (board.board[x + i][y - i] != color)
			return 0;

		value++;
	}
	return value;
}

int cn4::MiniMaxAi::rightDiagonalUpValue(const cn4::Board& board, int x, int y)
{
	if (x + 3 >= board.width)
		return 0;
	if (y + 3 >= board.height)
		return 0;

	int value = 0;
	int color = board.board[x][y];

	for (int i = 1; i < 4; i++) {
		if (board.board[x + i][y + i] == (int)cn4::PlayerColor::NONE)
			continue;

		if (board.board[x + i][y + i] != color)
			return 0;

		value++;
	}

	return value;
}

int cn4::MiniMaxAi::upVerticalValue(const cn4::Board& board, int x, int y)
{
	if (y + 3 >= board.height)
		return 0;

	int value = 0;
	int color = board.board[x][y];

	for (int i = 1; i < 4; i++) {
		if (board.board[x][y + i] == (int)cn4::PlayerColor::NONE)
			return value;

		if (board.board[x][y + i] != color)
			return 0;

		value++;
	}
	return value;
}

std::vector<int> cn4::MiniMaxAi::getBestMoves(std::vector<int>& moves, std::vector<int>& values)
{
	std::vector <int> bestMoves;
	int highestValue = *(std::max_element(values.begin(), values.end()));

	for (int i = 0; i < moves.size(); i++) {
		if (values[i] == highestValue)
			bestMoves.push_back(moves[i]);
	}

	return bestMoves;
}

int cn4::MiniMaxAi::getRandomMove(const std::vector<int>& moves)
{
	int randIndex = rand() % moves.size();
	return moves[randIndex];
}

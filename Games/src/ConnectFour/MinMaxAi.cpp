#include "ConnectFour/MinMaxAi.h"


cn4::MinMaxAi::MinMaxAi(int depth) {
	this->depth = depth;
	ConnectFourAdapter adap = ConnectFourAdapter();
}

cn4::MinMaxAi::~MinMaxAi() {

}

int cn4::MinMaxAi::getMove(std::string state, int playerColor) {
	aiColor = playerColor;


	cn4::Board board = adap.convertStringToBoard(state);
	std::vector<int> possibleMoves = getAllPossibleMoves(board);
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

void cn4::MinMaxAi::makeMove(cn4::Board& board, int move, int currenPlayer) {
	for (int i = 0; i < board.height; i++) {
		if (board.board[move][i] == 0) {
			board.board[move][i] = currenPlayer;
			return;
		}
	}
}

int cn4::MinMaxAi::evaluateBoard(cn4::Board& board, int depth, int currentPlayer, bool maximizingPlayer, int alpha,
	int beta) {

	int nextPlayer = adap.getNextPlayer(currentPlayer);

	if (cn4::GameLogic::isDraw(board))
		return 0;
	else if (cn4::GameLogic::getPlayerWon(board) == aiColor)
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


	std::vector<int> possibleMoves = getAllPossibleMoves(board);
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

std::vector<int> cn4::MinMaxAi::getAllPossibleMoves(const cn4::Board& board) {
	std::vector<int> possibleMoves;

	for (int i = 0; i < board.width; i++) {
		if (board.board[i][board.height - 1] == cn4::PlayerColor::NONE)
			possibleMoves.push_back(i);
	}


	return possibleMoves;
}

int cn4::MinMaxAi::staticBoardEvaluation(const cn4::Board& board) {
	int boardValue = 0;
	for (int x = 0; x < board.width; x++) {
		for (int y = 0; y < board.height; y++) {
			boardValue += evaluateBoardPosition(board, x, y);
		}
	}

	return boardValue;
}

int
cn4::MinMaxAi::evaluateBoardPosition(const cn4::Board& board, int x, int y) {

	int value = rightDiagonalUpValue(board, x, y) + rightDiagonalDownValue(board, x, y)
		+ rightHorizontalValue(board, x, y) + upVerticalValue(board, x, y);
	if (board.board[x][y] != aiColor)
		return -value;

	return value;
}

int cn4::MinMaxAi::rightHorizontalValue(const cn4::Board& board, int x, int y) {
	if (x + 3 >= board.width)
		return 0;

	int value = 0;
	int color = board.board[x][y];
	for (int i = 1; i < 4; i++) {
		if (board.board[x + i][y] == cn4::PlayerColor::NONE)
			continue;
		if (board.board[x + i][y] != color)
			return 0;

		value++;
	}

	return value;
}

int cn4::MinMaxAi::rightDiagonalDownValue(const cn4::Board& board, int x, int y) {

	if (x + 3 >= board.width)
		return 0;
	if (y - 3 < 0)
		return 0;

	int value = 0;
	int color = board.board[x][y];

	for (int i = 1; i < 4; i++) {
		if (board.board[x + i][y - i] == cn4::PlayerColor::NONE)
			continue;
		if (board.board[x + i][y - i] != color)
			return 0;

		value++;
	}
	return value;
}

int cn4::MinMaxAi::rightDiagonalUpValue(const cn4::Board& board, int x, int y) {
	if (x + 3 >= board.width)
		return 0;
	if (y + 3 >= board.height)
		return 0;

	int value = 0;
	int color = board.board[x][y];

	for (int i = 1; i < 4; i++) {
		if (board.board[x + i][y + i] == cn4::PlayerColor::NONE)
			continue;

		if (board.board[x + i][y + i] != color)
			return 0;

		value++;
	}

	return value;
}

int cn4::MinMaxAi::upVerticalValue(const cn4::Board& board, int x, int y) {
	if (y + 3 >= board.height)
		return 0;

	int value = 0;
	int color = board.board[x][y];

	for (int i = 1; i < 4; i++) {
		if (board.board[x][y + i] == cn4::PlayerColor::NONE)
			return value;

		if (board.board[x][y + i] != color)
			return 0;

		value++;
	}
	return value;
}

std::vector<int> cn4::MinMaxAi::getBestMoves(std::vector<int>& moves, std::vector<int>& values) {
	std::vector <int> bestMoves;
	int highestValue = *(std::max_element(values.begin(), values.end()));

	for (int i = 0; i < moves.size(); i++) {
		if (values[i] == highestValue)
			bestMoves.push_back(moves[i]);
	}

	return bestMoves;
}

int cn4::MinMaxAi::getRandomMove(const std::vector<int>& moves) {
	int randIndex = rand() % moves.size();
	return moves[randIndex];
}

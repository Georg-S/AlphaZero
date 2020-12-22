#include "TicTacToe/TicTacToeAdapter.h"

TicTacToeAdapter::TicTacToeAdapter() {

}

int TicTacToeAdapter::getInitialPlayer() {
	return 1;
}

std::string TicTacToeAdapter::getInitialGameState() {
	return initialState;
}

std::vector<int> TicTacToeAdapter::getAllPossibleMoves(const std::string& state, int currentPlayer) {
	ttt::Board board = convertStringToBoard(state);

	std::vector<int> possibleMoves;
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			if (board.board[x][y] == 0)
				possibleMoves.push_back(x + y * 3);
		}
	}
	return possibleMoves;
}

torch::Tensor TicTacToeAdapter::convertStateToNeuralNetInput(const std::string& state, int currentPlayer, torch::Device device) {
	ttt::Board board = convertStringToBoard(state);
	int otherPlayer = getNextPlayer(currentPlayer);
	torch::Tensor neuralInput = torch::zeros({ 1,2,3,3 });

	for (int x = 0; x < 3; x++) {
		for (int y = 0; y < 3; y++) {
			if (currentPlayer == board.board[x][y])
				neuralInput[0][0][x][y] = 1;
			else if (otherPlayer == board.board[x][y])
				neuralInput[0][1][x][y] = 1;
		}
	}
	neuralInput = neuralInput.to(device);
	return neuralInput;
}

int TicTacToeAdapter::getNextPlayer(int currentPlayer) {
	return currentPlayer % 2 + 1;
}

int TicTacToeAdapter::gameOverReward(const std::string& state, int currentPlayer) {
	int playerWon = getPlayerWon(state);

	if (playerWon == currentPlayer)
		return 1;
	else if (playerWon != 0)
		return -1;
	return 0;
}

bool TicTacToeAdapter::isGameOver(const std::string& state) {
	ttt::Board board = convertStringToBoard(state);
	return ttt::GameLogic::isGameOver(board);
}

std::string TicTacToeAdapter::makeMove(const std::string& state, int move, int currentPlayer) {
	ttt::Board board = convertStringToBoard(state);

	int x = move % 3;
	int y = move / 3;
	if (board.board[x][y] != 0 || (x < 0) || (x >= 3) || (y < 0) || (y >= 3)) {
		std::cout << "invalid move" << std::endl;
		return state;
	}
	board.board[x][y] = currentPlayer;

	return board.toString();
}

ttt::Board TicTacToeAdapter::convertStringToBoard(const std::string& state) {
	ttt::Board board = ttt::Board();
	char zeroOffset = '0';

	for (int x = 0; x < 3; x++)
		for (int y = 0; y < 3; y++)
			board.board[x][y] = state.at(y * 3 + x) - zeroOffset;

	return board;
}

int TicTacToeAdapter::getPlayerWon(const std::string& state) {
	ttt::Board board = convertStringToBoard(state);
	return ttt::GameLogic::getPlayerWon(board);
}

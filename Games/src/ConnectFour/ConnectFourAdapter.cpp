#include "ConnectFour/ConnectFourAdapter.h"

ConnectFourAdapter::ConnectFourAdapter()
{

}

std::string ConnectFourAdapter::makeMove(const std::string& state, int move, int currentPlayer)
{
	cn4::Board board = convertStringToBoard(state);
	cn4::PlayerColor playerColor = (cn4::PlayerColor)currentPlayer;
	cn4::GameLogic::makeMove(board, move, playerColor);
	return board.toString();
}

int ConnectFourAdapter::getActionCount() const
{
	return this->actionCount;
}

cn4::Board ConnectFourAdapter::convertStringToBoard(const std::string& state)
{
	cn4::Board result = cn4::Board();

	for (int index = 0; index < state.size(); index++) {
		int x = index % result.width;
		int y = index / result.width;

		result.board[x][y] = state.at(index) - '0';
	}

	return result;
}

std::string ConnectFourAdapter::getInitialGameState()
{
	static const std::string startState = "000000000000000000000000000000000000000000";
	return startState;
}

bool ConnectFourAdapter::isGameOver(const std::string& state)
{
	cn4::Board board = convertStringToBoard(state);
	return cn4::GameLogic::isGameOver(board);
}

int ConnectFourAdapter::gameOverReward(const std::string& state, int currentPlayer)
{
	int playerWon = getPlayerWon(state);

	if (playerWon == currentPlayer)
		return 1;
	else if (playerWon != 0)
		return -1;
	return 0;
}

int ConnectFourAdapter::getPlayerWon(const std::string& state)
{
	cn4::Board board = convertStringToBoard(state);
	int  winner = (int)cn4::GameLogic::getPlayerWon(board);

	return winner;
}

int ConnectFourAdapter::getNextPlayer(int currentPlayer)
{
	return currentPlayer % 2 + 1;
}

int ConnectFourAdapter::getInitialPlayer()
{
	return 1;
}

std::vector<int> ConnectFourAdapter::getAllPossibleMoves(const std::string& state, int currentPlayer)
{
	std::vector<int> possibleMoves;
	cn4::Board board = convertStringToBoard(state);

	for (int i = 0; i < actionCount; i++) {
		if (cn4::GameLogic::isMovePossible(board, i))
			possibleMoves.push_back(i);
	}

	return possibleMoves;
}

torch::Tensor ConnectFourAdapter::convertStateToNeuralNetInput(const std::string& state, int currentPlayer,
	torch::Device device)
{
	cn4::Board board = convertStringToBoard(state);
	int otherPlayer = currentPlayer % 2 + 1;
	torch::Tensor neuralInput = torch::zeros({ 1,2,board.width, board.height });

	for (int x = 0; x < board.width; x++) {
		for (int y = 0; y < board.height; y++) {
			if (currentPlayer == board.board[x][y])
				neuralInput[0][0][x][y] = 1;
			else if (otherPlayer == board.board[x][y])
				neuralInput[0][1][x][y] = 1;
		}
	}
	neuralInput = neuralInput.to(device);
	return neuralInput;
}

#include "ConnectFour/ConnectFourAdapter.h"

using namespace cn4;

std::string ConnectFourAdapter::makeMove(const std::string& state, int move, int currentPlayer)
{
	Board board = Board(state);
	PlayerColor playerColor = PlayerColor(currentPlayer);
	board.makeMove(move, playerColor);

	return board.toString();
}

int ConnectFourAdapter::getActionCount() const
{
	return m_actionCount;
}

std::string ConnectFourAdapter::getInitialGameState()
{
	static const std::string startState = "000000000000000000000000000000000000000000";
	return startState;
}

bool ConnectFourAdapter::isGameOver(const std::string& state)
{
	Board board = Board(state);
	return cn4::isGameOver(board);
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
	cn4::Board board = Board(state);

	return static_cast<int>(cn4::getPlayerWon(board));
}

int ConnectFourAdapter::getNextPlayer(int currentPlayer)
{
	return cn4::getNextPlayer(currentPlayer);
}

int ConnectFourAdapter::getInitialPlayer()
{
	return static_cast<int>(PlayerColor::YELLOW);
}

std::vector<int> ConnectFourAdapter::getAllPossibleMoves(const std::string& state, int currentPlayer)
{
	Board board = Board(state);

	return cn4::getAllPossibleMoves(board);
}

torch::Tensor ConnectFourAdapter::convertStateToNeuralNetInput(const std::string& state, int currentPlayer)
{
	Board board = Board(state);
	PlayerColor playerColor = PlayerColor(currentPlayer);
	PlayerColor otherPlayer = cn4::getNextPlayer(playerColor);
	torch::Tensor neuralInput = torch::zeros({ 1,2, boardWidth, boardHeight });

	for (int x = 0; x < boardWidth; x++)
	{
		for (int y = 0; y < boardHeight; y++)
		{
			if (playerColor == board.at(x, y))
				neuralInput[0][0][x][y] = 1;
			else if (otherPlayer == board.at(x, y))
				neuralInput[0][1][x][y] = 1;
		}
	}

	return neuralInput;
}

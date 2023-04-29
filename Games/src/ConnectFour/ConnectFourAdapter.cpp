#include "ConnectFour/ConnectFourAdapter.h"

using namespace cn4;

std::string ConnectFourAdapter::makeMove(const std::string& state, int move, int currentPlayer)
{
	auto board = makeMove(Board(state), move, currentPlayer);

	return board.toString();
}

cn4::Board ConnectFourAdapter::makeMove(const cn4::Board& board, int move, int currentPlayer) const
{
	Board result = board;
	result.makeMove(move, PlayerColor(currentPlayer));

	return result;
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
	return isGameOver(Board(state));
}

bool ConnectFourAdapter::isGameOver(const cn4::Board& board) const
{
	return cn4::isGameOver(board);
}

int ConnectFourAdapter::getPlayerWon(const std::string& state)
{
	return getPlayerWon(Board(state));
}

int ConnectFourAdapter::getPlayerWon(const cn4::Board& board) const
{
	return static_cast<int>(cn4::getPlayerWon(board));
}

int ConnectFourAdapter::gameOverReward(const std::string& state, int currentPlayer)
{
	return gameOverReward(Board(state), currentPlayer);
}

int ConnectFourAdapter::gameOverReward(const cn4::Board& board, int currentPlayer) const
{
	int playerWon = static_cast<int>(cn4::getPlayerWon(board));

	if (playerWon == currentPlayer)
		return 1;
	else if (playerWon != 0)
		return -1;
	return 0;
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
	return getAllPossibleMoves(Board(state), currentPlayer);
}

std::vector<int> ConnectFourAdapter::getAllPossibleMoves(const cn4::Board& board, int currentPlayer) const
{
	return cn4::getAllPossibleMoves(board);
}

torch::Tensor ConnectFourAdapter::convertStateToNeuralNetInput(const std::string& state, int currentPlayer)
{
	torch::Tensor neuralInput = torch::zeros({ 1,2, boardWidth, boardHeight });
	convertStateToNeuralNetInput(state, currentPlayer, neuralInput[0]);

	return neuralInput;
}

torch::Tensor ConnectFourAdapter::convertStateToNeuralNetInput(const cn4::Board& board, int currentPlayer) const
{
	torch::Tensor neuralInput = torch::zeros({ 1,2, boardWidth, boardHeight });
	convertStateToNeuralNetInput(board, currentPlayer, neuralInput[0]);

	return neuralInput;
}

void ConnectFourAdapter::convertStateToNeuralNetInput(const std::string& state, int currentPlayer, torch::Tensor outTensor)
{
	Board board = Board(state);
	convertStateToNeuralNetInput(board, currentPlayer, outTensor);
}

void ConnectFourAdapter::convertStateToNeuralNetInput(const cn4::Board& board, int currentPlayer, torch::Tensor outTensor) const
{
	PlayerColor playerColor = PlayerColor(currentPlayer);
	PlayerColor otherPlayer = cn4::getNextPlayer(playerColor);

	outTensor.zero_();
	for (int x = 0; x < boardWidth; x++)
	{
		for (int y = 0; y < boardHeight; y++)
		{
			if (playerColor == board.at(x, y))
				outTensor[0][x][y] = 1;
			else if (otherPlayer == board.at(x, y))
				outTensor[1][x][y] = 1;
		}
	}
}

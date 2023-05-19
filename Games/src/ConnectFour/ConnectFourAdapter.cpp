#include "ConnectFour/ConnectFourAdapter.h"

using namespace cn4;

int ConnectFourAdapter::getInitialPlayer() const
{
	return static_cast<int>(PlayerColor::YELLOW);
}

int ConnectFourAdapter::getNextPlayer(int currentPlayer) const
{
	return cn4::getNextPlayer(currentPlayer);
}

Board ConnectFourAdapter::getInitialGameState() const
{
	static const std::string startState = "000000000000000000000000000000000000000000";
	return Board(startState);
}

int ConnectFourAdapter::getPlayerWon(const cn4::Board& board) const
{
	return static_cast<int>(cn4::getPlayerWon(board));
}

torch::Tensor ConnectFourAdapter::convertStateToNeuralNetInput(const cn4::Board& board, int currentPlayer) const
{
	torch::Tensor neuralInput = torch::zeros({ 1,2, boardWidth, boardHeight });
	convertStateToNeuralNetInput(board, currentPlayer, neuralInput[0]);

	return neuralInput;
}

void ConnectFourAdapter::convertStateToNeuralNetInput(const cn4::Board& board, int currentPlayer, torch::Tensor outTensor) const
{
	PlayerColor playerColor = PlayerColor(currentPlayer);
	PlayerColor otherPlayer = cn4::getNextPlayer(playerColor);
	// Use accessor instead of accessing the data direct, this is way better performance wise
	auto outTensorAccessor = outTensor.accessor<float, 3>(); 

	outTensor.zero_();
	for (int x = 0; x < boardWidth; x++)
	{
		for (int y = 0; y < boardHeight; y++)
		{
			if (playerColor == board.at(x, y))
				outTensorAccessor[0][x][y] = 1;
			else if (otherPlayer == board.at(x, y))
				outTensorAccessor[1][x][y] = 1;
		}
	}
}

std::vector<int> ConnectFourAdapter::getAllPossibleMoves(const cn4::Board& board, int currentPlayer) const
{
	return cn4::getAllPossibleMoves(board);
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

bool ConnectFourAdapter::isGameOver(const cn4::Board& board) const
{
	return cn4::isGameOver(board);
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

cn4::Board ConnectFourAdapter::getGameStateFromString(const std::string str, int currentPlayer) const
{
	return cn4::Board(str);
}

std::string ConnectFourAdapter::getStringFromGameState(const cn4::Board& board) const
{
	return board.toString();
}

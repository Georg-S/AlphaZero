#include "TicTacToe/TicTacToeAdapter.h"

using namespace ttt;

int TicTacToeAdapter::getInitialPlayer()
{
	constexpr int initialPlayer = static_cast<int>(PlayerColor::CROSS);
	return initialPlayer;
}

std::string TicTacToeAdapter::getInitialGameState()
{
	return "---------";
}

std::vector<int> TicTacToeAdapter::getAllPossibleMoves(const std::string& state, int currentPlayer)
{
	return ttt::getAllPossibleMoves<int>(Board(state));
}

torch::Tensor TicTacToeAdapter::convertStateToNeuralNetInput(const std::string& state, int currentPlayer)
{
	return convertStateToNeuralNetInput(Board(state), currentPlayer);
}

void TicTacToeAdapter::convertStateToNeuralNetInput(const std::string& state, int currentPlayer, torch::Tensor outTensor)
{
	convertStateToNeuralNetInput(Board(state), currentPlayer, outTensor);
}

torch::Tensor TicTacToeAdapter::convertStateToNeuralNetInput(const ttt::Board& board, int currentPlayer) const
{
	torch::Tensor neuralInput = torch::zeros({ 1,2,3,3 });
	convertStateToNeuralNetInput(board, currentPlayer, neuralInput[0]);

	return neuralInput;
}

void TicTacToeAdapter::convertStateToNeuralNetInput(const ttt::Board& board, int currentPlayer, torch::Tensor outTensor) const
{
	PlayerColor playercolor = PlayerColor(currentPlayer);
	PlayerColor otherPlayer = ttt::getNextPlayer(playercolor);

	outTensor.zero_();
	for (int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 3; y++)
		{
			if (playercolor == board.at(x, y))
				outTensor[0][x][y] = 1;
			else if (otherPlayer == board.at(x, y))
				outTensor[1][x][y] = 1;
		}
	}
}

std::vector<int> TicTacToeAdapter::getAllPossibleMoves(const ttt::Board& board, int currentPlayer) const
{
	return ttt::getAllPossibleMoves<int>(board);
}

int TicTacToeAdapter::getNextPlayer(int currentPlayer)
{
	return static_cast<int>(ttt::getNextPlayer(PlayerColor(currentPlayer)));
}

int TicTacToeAdapter::gameOverReward(const std::string& state, int currentPlayer)
{
	int playerWon = getPlayerWon(state);

	if (playerWon == currentPlayer)
		return 1;
	else if (playerWon != 0)
		return -1;
	return 0;
}

bool TicTacToeAdapter::isGameOver(const std::string& state)
{
	Board board = Board(state);

	return ttt::isGameOver(board);
}

std::string TicTacToeAdapter::makeMove(const std::string& state, int move, int currentPlayer)
{
	Board board = Board(state);
	board.makeMove(move, PlayerColor(currentPlayer));

	return board.toString();
}

int TicTacToeAdapter::getActionCount() const
{
	return m_actionCount;
}

int TicTacToeAdapter::getPlayerWon(const std::string& state)
{
	Board board = Board(state);

	if (playerWon(board, PlayerColor::CROSS))
		return static_cast<int>(PlayerColor::CROSS);
	if (playerWon(board, PlayerColor::DOT))
		return static_cast<int>(PlayerColor::DOT);

	return static_cast<int>(PlayerColor::NONE);
}

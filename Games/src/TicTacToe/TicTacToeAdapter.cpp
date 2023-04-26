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
	return gameOverReward(ttt::Board(state), currentPlayer);
}

int TicTacToeAdapter::gameOverReward(const ttt::Board& board, int currentPlayer) const
{
	int playerWon = getPlayerWon(board);

	if (playerWon == currentPlayer)
		return 1;
	else if (playerWon != 0)
		return -1;
	return 0;
}

bool TicTacToeAdapter::isGameOver(const std::string& state)
{
	return isGameOver(Board(state));
}

bool TicTacToeAdapter::isGameOver(const ttt::Board& board) const
{
	return ttt::isGameOver(board);
}

std::string TicTacToeAdapter::makeMove(const std::string& state, int move, int currentPlayer)
{
	auto result = makeMove(ttt::Board(state), move, currentPlayer);

	return result.toString();
}

ttt::Board TicTacToeAdapter::makeMove(ttt::Board board, int move, int currentPlayer) const
{
	board.makeMove(move, PlayerColor(currentPlayer));

	return board;
}

int TicTacToeAdapter::getActionCount() const
{
	return m_actionCount;
}

int TicTacToeAdapter::getPlayerWon(const std::string& state)
{
	return getPlayerWon(Board(state));
}

int TicTacToeAdapter::getPlayerWon(const ttt::Board& board) const
{
	if (playerWon(board, PlayerColor::CROSS))
		return static_cast<int>(PlayerColor::CROSS);
	if (playerWon(board, PlayerColor::DOT))
		return static_cast<int>(PlayerColor::DOT);

	return static_cast<int>(PlayerColor::NONE);
}

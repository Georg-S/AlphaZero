#ifndef DEEPREINFORCEMENTLEARNING_TICTACTOEADAPTER_H
#define DEEPREINFORCEMENTLEARNING_TICTACTOEADAPTER_H

#include <vector>
// Libtorch has many warnings which clutter the output, so we ignore them
#pragma warning(push, 0)
#include <torch/torch.h>
#pragma warning(pop)

#include "GameLogic.h"

class TicTacToeAdapter
{
public:
	TicTacToeAdapter() = default;
	int getInitialPlayer() const;
	int getNextPlayer(int currentPlayer) const;
	ttt::Board getInitialGameState() const;
	int getPlayerWon(const ttt::Board& board) const;
	torch::Tensor convertStateToNeuralNetInput(const ttt::Board& board, int currentPlayer) const;
	void convertStateToNeuralNetInput(const ttt::Board& board, int currentPlayer, torch::Tensor outTensor) const;
	std::vector<int> getAllPossibleMoves(const ttt::Board& board, int currentPlayer) const;
	int gameOverReward(const ttt::Board& board, int currentPlayer) const;
	bool isGameOver(const ttt::Board& board) const;
	ttt::Board makeMove(ttt::Board board, int move, int currentPlayer) const;
	int getActionCount() const;
	ttt::Board getGameStateFromString(const std::string str, int currentPlayer);
	std::string getStringFromGameState(const ttt::Board& board);
private:
	static constexpr int m_actionCount = 9;
};


#endif //DEEPREINFORCEMENTLEARNING_TICTACTOEADAPTER_H

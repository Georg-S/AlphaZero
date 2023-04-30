#ifndef DEEPREINFORCEMENTLEARNING_TICTACTOEADAPTER_H
#define DEEPREINFORCEMENTLEARNING_TICTACTOEADAPTER_H

#include <vector>
#include <torch/torch.h>
#include "GameLogic.h"

class TicTacToeAdapter
{
public:
	TicTacToeAdapter() = default;
	int getInitialPlayer() const;
	int getPlayerWon(const ttt::Board& board) const;
	ttt::Board getInitialGameState() const;
	torch::Tensor convertStateToNeuralNetInput(const ttt::Board& board, int currentPlayer) const;
	void convertStateToNeuralNetInput(const ttt::Board& board, int currentPlayer, torch::Tensor outTensor) const;
	std::vector<int> getAllPossibleMoves(const ttt::Board& board, int currentPlayer) const;
	int getNextPlayer(int currentPlayer) const;
	int gameOverReward(const ttt::Board& board, int currentPlayer) const;
	bool isGameOver(const ttt::Board& board) const;
	ttt::Board makeMove(ttt::Board board, int move, int currentPlayer) const;
	int getActionCount() const;

private:
	static constexpr int m_actionCount = 9;
};


#endif //DEEPREINFORCEMENTLEARNING_TICTACTOEADAPTER_H

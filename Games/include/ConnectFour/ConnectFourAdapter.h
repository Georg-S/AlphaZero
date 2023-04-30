#ifndef DEEPREINFORCEMENTLEARNING_CONNECTFOURADAPTER_H
#define DEEPREINFORCEMENTLEARNING_CONNECTFOURADAPTER_H

#include <torch/torch.h>
#include <string>
#include <vector>
#include "GameLogic.h"

class ConnectFourAdapter
{
public:
	ConnectFourAdapter() = default;
	int getInitialPlayer() const;
	int getNextPlayer(int currentPlayer) const;
	cn4::Board getInitialGameState() const;
	int getPlayerWon(const cn4::Board& board) const;
	torch::Tensor convertStateToNeuralNetInput(const cn4::Board& board, int currentPlayer) const;
	void convertStateToNeuralNetInput(const cn4::Board& board, int currentPlayer, torch::Tensor outTensor) const;
	std::vector<int> getAllPossibleMoves(const cn4::Board& board, int currentPlayer) const;
	int gameOverReward(const cn4::Board& board, int currentPlayer) const;
	bool isGameOver(const cn4::Board& board) const;
	cn4::Board makeMove(const cn4::Board& board, int move, int currentPlayer) const;
	int getActionCount() const ;
private:
	static constexpr int m_actionCount = 7;
};

#endif //DEEPREINFORCEMENTLEARNING_CONNECTFOURADAPTER_H

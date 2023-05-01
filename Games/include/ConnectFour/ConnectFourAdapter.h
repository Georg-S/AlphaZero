#ifndef DEEPREINFORCEMENTLEARNING_CONNECTFOURADAPTER_H
#define DEEPREINFORCEMENTLEARNING_CONNECTFOURADAPTER_H

#include <string>
#include <vector>
// Libtorch has many warnings which clutter the output, so we ignore them
#pragma warning(push, 0)
#include <torch/torch.h>
#pragma warning(pop)

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
	int getActionCount() const;
	cn4::Board getGameStateFromString(const std::string str, int currentPlayer) const;
	std::string getStringFromGameState(const cn4::Board& board) const;
private:
	static constexpr int m_actionCount = 7;
};

#endif //DEEPREINFORCEMENTLEARNING_CONNECTFOURADAPTER_H

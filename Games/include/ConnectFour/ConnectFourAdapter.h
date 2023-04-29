#ifndef DEEPREINFORCEMENTLEARNING_CONNECTFOURADAPTER_H
#define DEEPREINFORCEMENTLEARNING_CONNECTFOURADAPTER_H

#include <string>
#include <vector>
#include <Game.h>
#include "GameLogic.h"

class ConnectFourAdapter : public Game
{
public:
	ConnectFourAdapter() = default;
	std::vector<int> getAllPossibleMoves(const std::string& state, int currentPlayer) override;
	std::vector<int> getAllPossibleMoves(const cn4::Board& board, int currentPlayer) const;
	torch::Tensor convertStateToNeuralNetInput(const std::string& state, int currentPlayer) override;
	torch::Tensor convertStateToNeuralNetInput(const cn4::Board& board, int currentPlayer) const;
	void convertStateToNeuralNetInput(const std::string& state, int currentPlayer, torch::Tensor outTensor) override;
	void convertStateToNeuralNetInput(const cn4::Board& board, int currentPlayer, torch::Tensor outTensor) const;
	std::string getInitialGameState() override;
	int getInitialPlayer() override;
	int getNextPlayer(int currentPlayer) override;
	int getPlayerWon(const std::string& state) override;
	int getPlayerWon(const cn4::Board& board) const;
	int gameOverReward(const std::string& state, int currentPlayer) override;
	int gameOverReward(const cn4::Board& board, int currentPlayer) const;
	bool isGameOver(const std::string& state) override;
	bool isGameOver(const cn4::Board& board) const;
	std::string makeMove(const std::string& state, int move, int currentPlayer) override;
	cn4::Board makeMove(const cn4::Board& board, int move, int currentPlayer) const;
	int getActionCount() const override;
private:
	static constexpr int m_actionCount = 7;
};

#endif //DEEPREINFORCEMENTLEARNING_CONNECTFOURADAPTER_H

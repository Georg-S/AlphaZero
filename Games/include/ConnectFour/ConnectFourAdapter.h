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
	torch::Tensor convertStateToNeuralNetInput(const std::string& state, int currentPlayer) override;
	void convertStateToNeuralNetInput(const std::string& state, int currentPlayer, torch::Tensor outTensor) override;
	std::string getInitialGameState() override;
	int getInitialPlayer() override;
	int getNextPlayer(int currentPlayer) override;
	int getPlayerWon(const std::string& state) override;
	int gameOverReward(const std::string& state, int currentPlayer) override;
	bool isGameOver(const std::string& state) override;
	std::string makeMove(const std::string& state, int move, int currentPlayer) override;
	int getActionCount() const override;
private:
	static constexpr int m_actionCount = 7;
};

#endif //DEEPREINFORCEMENTLEARNING_CONNECTFOURADAPTER_H

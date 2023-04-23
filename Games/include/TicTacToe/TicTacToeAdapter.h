#ifndef DEEPREINFORCEMENTLEARNING_TICTACTOEADAPTER_H
#define DEEPREINFORCEMENTLEARNING_TICTACTOEADAPTER_H

#include <string>
#include <vector>
#include <Game.h>
#include "GameLogic.h"

class TicTacToeAdapter : public Game
{
public:
	TicTacToeAdapter() = default;
	int getInitialPlayer() override;
	int getPlayerWon(const std::string& state) override;
	std::string getInitialGameState() override;
	torch::Tensor convertStateToNeuralNetInput(const std::string& state, int currentPlayer) override;
	void convertStateToNeuralNetInput(const std::string& state, int currentPlayer, torch::Tensor outTensor) override;
	std::vector<int> getAllPossibleMoves(const std::string& state, int currentPlayer) override;
	int getNextPlayer(int currentPlayer) override;
	int gameOverReward(const std::string& state, int currentPlayer) override;
	bool isGameOver(const std::string& state) override;
	std::string makeMove(const std::string& state, int move, int currentPlayer) override;
	int getActionCount() const override;

private:
	static constexpr int m_actionCount = 9;
};


#endif //DEEPREINFORCEMENTLEARNING_TICTACTOEADAPTER_H

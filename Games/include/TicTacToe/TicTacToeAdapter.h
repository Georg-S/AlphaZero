#ifndef DEEPREINFORCEMENTLEARNING_TICTACTOEADAPTER_H
#define DEEPREINFORCEMENTLEARNING_TICTACTOEADAPTER_H

#include <string>
#include <vector>
#include <Game.h>
#include "GameLogic.h"
#include "Board.h"

class TicTacToeAdapter : public Game
{
public:
	TicTacToeAdapter();
	int getInitialPlayer() override;
	int getPlayerWon(const std::string& state) override;
	std::string getInitialGameState() override;
	torch::Tensor convertStateToNeuralNetInput(const std::string& state, int currentPlayer, torch::Device device = torch::kCPU) override;
	std::vector<int> getAllPossibleMoves(const std::string& state, int currentPlayer);
	int getNextPlayer(int currentPlayer) override;
	int gameOverReward(const std::string& state, int currentPlayer) override;
	bool isGameOver(const std::string& state) override;
	std::string makeMove(const std::string& state, int move, int currentPlayer) override;
	ttt::Board convertStringToBoard(const std::string& state);
	int getActionCount() const override;

private:
	static constexpr int actionCount = 9;
};


#endif //DEEPREINFORCEMENTLEARNING_TICTACTOEADAPTER_H

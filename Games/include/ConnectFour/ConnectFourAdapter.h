#ifndef DEEPREINFORCEMENTLEARNING_CONNECTFOURADAPTER_H
#define DEEPREINFORCEMENTLEARNING_CONNECTFOURADAPTER_H

#include <string>
#include <vector>
#include <Game.h>
#include "GameLogic.h"
#include "Board.h"

class ConnectFourAdapter : public Game {

public:
	ConnectFourAdapter();
	std::vector<int> getAllPossibleMoves(const std::string& state, int currentPlayer) override;
	torch::Tensor convertStateToNeuralNetInput(const std::string& state, int currentPlayer, torch::Device device = torch::kCPU) override;
	std::string getInitialGameState() override;
	int getInitialPlayer() override;
	int getNextPlayer(int currentPlayer) override;
	int getPlayerWon(const std::string& state) override;
	int gameOverReward(const std::string& state, int currentPlayer) override;
	bool isGameOver(const std::string& state) override;
	std::string makeMove(const std::string& state, int move, int currentPlayer) override;

	cn4::Board convertStringToBoard(const std::string& state);
private:
	static constexpr int actionCount = 7;
	inline static const std::string startState = "000000000000000000000000000000000000000000";
};

#endif //DEEPREINFORCEMENTLEARNING_CONNECTFOURADAPTER_H

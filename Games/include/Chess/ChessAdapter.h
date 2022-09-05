#ifndef DEEPREINFORCEMENTLEARNING_REDUCEDCHESSADAPTER_H
#define DEEPREINFORCEMENTLEARNING_REDUCEDCHESSADAPTER_H

#include <Game.h>
#include "Chess/Engine/ChessEngine.h"

namespace chess
{
	ceg::Move getMoveFromInt(int move);
	int getIntFromMove(const ceg::Move& move);
}

class ChessAdapter : public Game
{
public:
	ChessAdapter();
	std::vector<int> getAllPossibleMoves(const std::string& state, int currentPlayer) override;
	int getInitialPlayer() override;
	std::string getInitialGameState() override;
	int getPlayerWon(const std::string& state) override;
	int getNextPlayer(int currentPlayer) override;
	std::string makeMove(const std::string& state, int move, int currentPlayer) override;
	torch::Tensor convertStateToNeuralNetInput(const std::string& state, int currentPlayer, torch::Device device = torch::kCPU) override;
	bool isGameOver(const std::string& state) override;
	int gameOverReward(const std::string& state, int currentPlayer) override;
	int getActionCount() const override;

private:
	static constexpr int m_actionCount = 4096;
	std::unique_ptr<ceg::ChessEngine> chessEngine;
};


#endif //DEEPREINFORCEMENTLEARNING_REDUCEDCHESSADAPTER_H

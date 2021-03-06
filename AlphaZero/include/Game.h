#ifndef DEEPREINFORCEMENTLEARNING_GAME_H
#define DEEPREINFORCEMENTLEARNING_GAME_H
#include <string>
#include <torch/torch.h>
#include <vector>

class Game
{
public:
	virtual int getInitialPlayer() = 0;
	virtual std::string getInitialGameState() = 0;
	virtual bool isGameOver(const std::string& state) = 0;
	virtual int gameOverReward(const std::string& state, int currentPlayer) = 0;
	virtual int getPlayerWon(const std::string& state) = 0;
	virtual torch::Tensor convertStateToNeuralNetInput(const std::string& state, int currentPlayer, torch::Device device = torch::kCPU) = 0;
	virtual std::vector<int> getAllPossibleMoves(const std::string& state, int currentPlayer) = 0;
	virtual std::string makeMove(const std::string& state, int move, int currentPlayer) = 0;
	virtual int getNextPlayer(int currentPlayer) = 0;
	virtual int getActionCount() const = 0;
};

#endif //DEEPREINFORCEMENTLEARNING_GAME_H

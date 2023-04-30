#ifndef DEEPREINFORCEMENTLEARNING_TICTACTOEADAPTER_H
#define DEEPREINFORCEMENTLEARNING_TICTACTOEADAPTER_H

#include <string>
#include <vector>
#include <torch/torch.h>
#include "GameLogic.h"

class TicTacToeAdapter
{
public:
	TicTacToeAdapter() = default;
	int getInitialPlayer();
	int getPlayerWon(const std::string& state);
	int getPlayerWon(const ttt::Board& board) const;
	std::string getInitialGameState();
	torch::Tensor convertStateToNeuralNetInput(const std::string& state, int currentPlayer);
	void convertStateToNeuralNetInput(const std::string& state, int currentPlayer, torch::Tensor outTensor);
	torch::Tensor convertStateToNeuralNetInput(const ttt::Board& board, int currentPlayer) const;
	void convertStateToNeuralNetInput(const ttt::Board& board, int currentPlayer, torch::Tensor outTensor) const;
	std::vector<int> getAllPossibleMoves(const ttt::Board& board, int currentPlayer) const;
	std::vector<int> getAllPossibleMoves(const std::string& state, int currentPlayer);
	int getNextPlayer(int currentPlayer);
	int gameOverReward(const std::string& state, int currentPlayer);
	int gameOverReward(const ttt::Board& board, int currentPlayer) const;
	bool isGameOver(const std::string& state);
	bool isGameOver(const ttt::Board& board) const;
	std::string makeMove(const std::string& state, int move, int currentPlayer);
	ttt::Board makeMove(ttt::Board board, int move, int currentPlayer) const;
	int getActionCount() const;

private:
	static constexpr int m_actionCount = 9;
};


#endif //DEEPREINFORCEMENTLEARNING_TICTACTOEADAPTER_H

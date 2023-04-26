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
	int getPlayerWon(const ttt::Board& board) const;
	std::string getInitialGameState() override;
	torch::Tensor convertStateToNeuralNetInput(const std::string& state, int currentPlayer) override;
	void convertStateToNeuralNetInput(const std::string& state, int currentPlayer, torch::Tensor outTensor) override;
	torch::Tensor convertStateToNeuralNetInput(const ttt::Board& board, int currentPlayer) const;
	void convertStateToNeuralNetInput(const ttt::Board& board, int currentPlayer, torch::Tensor outTensor) const;
	std::vector<int> getAllPossibleMoves(const ttt::Board& board, int currentPlayer) const;
	std::vector<int> getAllPossibleMoves(const std::string& state, int currentPlayer) override;
	int getNextPlayer(int currentPlayer) override;
	int gameOverReward(const std::string& state, int currentPlayer) override;
	int gameOverReward(const ttt::Board& board, int currentPlayer) const;
	bool isGameOver(const std::string& state) override;
	bool isGameOver(const ttt::Board& board) const;
	std::string makeMove(const std::string& state, int move, int currentPlayer) override;
	ttt::Board makeMove(ttt::Board board, int move, int currentPlayer) const;
	int getActionCount() const override;

private:
	static constexpr int m_actionCount = 9;
};


#endif //DEEPREINFORCEMENTLEARNING_TICTACTOEADAPTER_H

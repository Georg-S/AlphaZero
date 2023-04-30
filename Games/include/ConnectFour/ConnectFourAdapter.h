#ifndef DEEPREINFORCEMENTLEARNING_CONNECTFOURADAPTER_H
#define DEEPREINFORCEMENTLEARNING_CONNECTFOURADAPTER_H

#include <torch/torch.h>
#include <string>
#include <vector>
#include "GameLogic.h"

class ConnectFourAdapter
{
public:
	ConnectFourAdapter() = default;
	std::vector<int> getAllPossibleMoves(const std::string& state, int currentPlayer) ;
	std::vector<int> getAllPossibleMoves(const cn4::Board& board, int currentPlayer) const;
	torch::Tensor convertStateToNeuralNetInput(const std::string& state, int currentPlayer) ;
	torch::Tensor convertStateToNeuralNetInput(const cn4::Board& board, int currentPlayer) const;
	void convertStateToNeuralNetInput(const std::string& state, int currentPlayer, torch::Tensor outTensor) ;
	void convertStateToNeuralNetInput(const cn4::Board& board, int currentPlayer, torch::Tensor outTensor) const;
	std::string getInitialGameState() ;
	int getInitialPlayer() ;
	int getNextPlayer(int currentPlayer) ;
	int getPlayerWon(const std::string& state) ;
	int getPlayerWon(const cn4::Board& board) const;
	int gameOverReward(const std::string& state, int currentPlayer) ;
	int gameOverReward(const cn4::Board& board, int currentPlayer) const;
	bool isGameOver(const std::string& state) ;
	bool isGameOver(const cn4::Board& board) const;
	std::string makeMove(const std::string& state, int move, int currentPlayer) ;
	cn4::Board makeMove(const cn4::Board& board, int move, int currentPlayer) const;
	int getActionCount() const ;
private:
	static constexpr int m_actionCount = 7;
};

#endif //DEEPREINFORCEMENTLEARNING_CONNECTFOURADAPTER_H

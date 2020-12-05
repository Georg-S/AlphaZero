#ifndef PROTOTYPE_TICTACTOEADAPTER_H
#define PROTOTYPE_TICTACTOEADAPTER_H

#include <string>
#include <vector>
#include <Game.h>
#include "GameLogic.h"
#include "Board.h"

class TicTacToeAdapter : public Game{
public:
    TicTacToeAdapter();
    ~TicTacToeAdapter();

    int getInitialPlayer() override ;
    int getPlayerWon(const std::string &state) override;
    std::string getInitialGameState() override ;
    torch::Tensor convertStateToNeuralNetInput(const std::string &state, const int &currentPlayer, torch::Device device = torch::kCPU) override;
    std::vector<int> getAllPossibleMoves(const std::string &state, const int &currentPlayer);
    int getNextPlayer(const int &currentPlayer) override;
    int gameOverReward(const std::string &state, const int &currentPlayer) override;
    bool isGameOver(const std::string &state) override;
    std::string makeMove(const std::string &state, const int &move, const int &currentPlayer) override;
    ttt::Board convertStringToBoard(const std::string &state);
    std::string convertBoardToString(const ttt::Board &board);
};


#endif //PROTOTYPE_TICTACTOEADAPTER_H

#ifndef PROTOTYPE_GAME_H
#define PROTOTYPE_GAME_H
#include <string>
#include <torch/torch.h>
#include <vector>

class Game{
public:
    virtual int getInitialPlayer() = 0;
    virtual std::string getInitialGameState() = 0;
    virtual bool isGameOver(const std::string &state) = 0;
    virtual int gameOverReward(const std::string &state, const int &currentPlayer) = 0;
    virtual int getPlayerWon(const std::string &state) = 0;
    virtual torch::Tensor convertStateToNeuralNetInput(const std::string &state, const int &currentPlayer, torch::Device device = torch::kCPU) = 0;
    virtual std::vector<int> getAllPossibleMoves(const std::string &state, const int &currentPlayer) = 0;
    virtual std::string makeMove(const std::string &state, const int &move, const int &currentPlayer) = 0;
    virtual int getNextPlayer(const int &currentPlayer) = 0;
};

#endif //PROTOTYPE_GAME_H

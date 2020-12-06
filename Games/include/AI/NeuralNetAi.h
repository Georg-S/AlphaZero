#ifndef DEEPREINFORCEMENTLEARNING_NEURALNETAI_H
#define DEEPREINFORCEMENTLEARNING_NEURALNETAI_H

#include <cfloat>
#include <MonteCarloTreeSearch.h>
#include <NeuralNetworks/NeuralNetwork.h>
#include <Game.h>
#include "Ai.h"


class NeuralNetAi : public Ai {

public:
    NeuralNetAi(NeuralNetwork* net, Game* game, int actionCount, int mctsCount = 50, bool probabilistic = false,  torch::DeviceType device = torch::kCPU);
    ~NeuralNetAi();

    int getMove(std::string state, int color) override;

private:
    NeuralNetwork* net;
    Game* game;
    int actionCount;
    int mctsCount;
    torch::DeviceType device;
    bool probabilistic = false;

    int getArgMaxIndex(const std::vector<float> &vec);
    int getProbabilisticAction(const std::vector<float> &vec);
};


#endif //DEEPREINFORCEMENTLEARNING_NEURALNETAI_H

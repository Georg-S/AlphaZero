#ifndef PROTOTYPE_NEURALNETAI_H
#define PROTOTYPE_NEURALNETAI_H

#include <cfloat>
#include <AlphaZero/MonteCarloTreeSearch.h>
#include <AlphaZero/NeuralNetworks/NeuralNetwork.h>
#include <AlphaZero/Game.h>
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


#endif //PROTOTYPE_NEURALNETAI_H

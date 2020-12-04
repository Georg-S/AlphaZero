#ifndef PROTOTYPE_MONTECARLOTREESEARCH_H
#define PROTOTYPE_MONTECARLOTREESEARCH_H

#include <map>
#include <vector>
#include <string>
#include <torch/torch.h>
#include <limits>
#include <utility>
#include "Game.h"
#include "NeuralNetworks/NeuralNetwork.h"

class MonteCarloTreeSearch {
public:
    MonteCarloTreeSearch();
    MonteCarloTreeSearch(int actionCount, float cpuct = 1.0);
    ~MonteCarloTreeSearch();

    void search(const int &count, std::string strState, NeuralNetwork* net, Game* game, int currentPlayer, torch::DeviceType device = torch::kCPU);
    float search(std::string strState, NeuralNetwork* net, Game* game, int currentPlayer, torch::DeviceType device = torch::kCPU);
    std::vector<float> getProbabilities(const std::string &state, const float &temperature = 1.0);
    static int sum(const std::vector<int> &vector);
    void clearAll();
    void fillQValuesAndVisitCount(const std::string &state);

private:
    float expandNewEncounteredState(const std::string &strState, const int &currentPlayer, Game* game, NeuralNetwork* net, torch::DeviceType device);
    int getActionWithHighestUpperConfidenceBound(const std::string &strState, const int &currentPlayer, Game* game);
    float calculateUpperConfidenceBound(const std::string &strState, const int &action);

    int actionCount;
    float cpuct;
    std::map<std::string, bool> loopDetection;
    std::map<std::string, bool> visited;
    std::map<std::string, std::vector<int>> visitCount;
    std::map<std::string, std::vector<float>> qValues;
    std::map<std::string, torch::Tensor> probabilities;
};


#endif //PROTOTYPE_MONTECARLOTREESEARCH_H

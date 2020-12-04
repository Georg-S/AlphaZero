#include "AI/NeuralNetAi.h"

NeuralNetAi::NeuralNetAi(NeuralNetwork *net, Game *game, int actionCount, int mctsCount, bool probabilistic,  torch::DeviceType device) {
    this->net = net;
    this->game = game;
    this->actionCount = actionCount;
    this->mctsCount = mctsCount;
    this->device = device;
    this->probabilistic = probabilistic;
}


NeuralNetAi::~NeuralNetAi() {

}

int NeuralNetAi::getMove(std::string state, int color) {
    MonteCarloTreeSearch mcts = MonteCarloTreeSearch(actionCount);
    mcts.search(mctsCount, state, net, game, color, device);
    std::vector<float> probs = mcts.getProbabilities(state);

    if(probabilistic)
        return getProbabilisticAction(probs);
    else
        return getArgMaxIndex(probs);
}

int NeuralNetAi::getArgMaxIndex(const std::vector<float> &vec) {
    return std::max_element(vec.begin(),vec.end()) - vec.begin();
}

int NeuralNetAi::getProbabilisticAction(const std::vector<float> &vec) {
    float r = ((float) rand() / (RAND_MAX));
    if(r == 0)
        r+=FLT_MIN;
    else if(r == 1)
        r-=FLT_MIN;
    float acc = 0.f;
    int x = 0;
    for(; x < vec.size(); x++) {
        acc+= vec[x];

        if(acc >= r)
            return x;
    }

    return x;
}

#ifndef PROTOTYPE_REPLAYELEMENT_H
#define PROTOTYPE_REPLAYELEMENT_H

struct ReplayElement{
    ReplayElement(std::string state, int currentPlayer, std::vector<float> probs, float result) {
        this->state = state;
        this->currentPlayer = currentPlayer;
        this->mctsProbabilities = probs;
        this->result = result;
    }

    std::string state;
    int currentPlayer;
    std::vector<float> mctsProbabilities;
    float result;
};


#endif //PROTOTYPE_REPLAYELEMENT_H

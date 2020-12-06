#ifndef DEEPREINFORCEMENTLEARNING_TRAINING_H
#define DEEPREINFORCEMENTLEARNING_TRAINING_H

#include <string>
#include <vector>
#include <time.h>
#include <cfloat>
#include "NeuralNetworks/NeuralNetwork.h"
#include "Game.h"
#include "RingBuffer.h"
#include "MonteCarloTreeSearch.h"
#include "ReplayElement.h"


class Training {
public:
    Training(int actionCount, NeuralNetwork* net);
    Training(int actionCount, NeuralNetwork* currentBest, NeuralNetwork* apprentice, torch::DeviceType = torch::kCPU);
    ~Training();

    void runTraining(Game* game);
    void selfPlay(NeuralNetwork* net, Game* game, MonteCarloTreeSearch mcts);
    std::vector<ReplayElement> selfPlayGame(NeuralNetwork* net, Game* game, MonteCarloTreeSearch mcts);
    static int getRandomAction(const std::vector<float> &probabilities);
    void addResult(std::vector<ReplayElement> &elements, const int &winner);
    void trainNet(NeuralNetwork* net, Game* game);
    torch::Tensor convertSampleToNeuralInput(const std::vector<ReplayElement> &sample, Game* game);
    torch::Tensor convertToValueTarget(const std::vector<ReplayElement> &sample);
    torch::Tensor convertToProbsTarget(const std::vector<ReplayElement> &sample);
    void evaluation(Game* game, const int &iteration);
    float evaluateApprentice(Game* game);
    int determinsticSelfPlayGame(NeuralNetwork* first, NeuralNetwork* second, Game* game, MonteCarloTreeSearch &mctsFirst, MonteCarloTreeSearch &mctsSecond);
    int getArgMaxIndex(const std::vector<float> &vec);
    void save(const int &iteration);

private:
    RingBuffer<ReplayElement> replayMemory;
    int actionCount = -1;
    torch::DeviceType device;
    NeuralNetwork* currentBest;
    NeuralNetwork* apprentice;


    //Training Flags
    const bool TRAINING_DONT_USE_DRAWS = false;
    const bool CLEAR_REPLAY_MEMORY_AFTER_IMPROVEMENT = true;
    const bool LOAD_OLD_NET_IF_NOT_IMPROVED = false;


    //Training Parameters
    const int TRAINING_ITERATIONS = 10000;
    const int MAX_REPLAY_MEMORY_SIZE = 30000;
    const int MIN_REPLAY_MEMORY_SIZE = 100;
    const int SELF_PLAY_MCTS_COUNT = 25;
    const int NUM_SELF_PLAY_GAMES = 100;
    const int EVALUATION_MCTS_COUNT = 25;
    const int TRAINING_ROUND_COUNT = 20;
//    const int TRAINING_ROUND_COUNT = 20;
    const int TRAINING_BATCH_SIZE = 400;
    const int SAVE_ITERATION_COUNT = 100;
    const int EVALUATION_STEP = 2;
    const int EVALUATION_GAMES = 2;
    const float EVALUATION_THRESHOLD = 0.6;
    const int RANDOM_MOVE_COUNT = 10;
};


#endif //DEEPREINFORCEMENTLEARNING_TRAINING_H

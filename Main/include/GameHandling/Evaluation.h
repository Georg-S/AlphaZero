#ifndef DEEPREINFORCEMENTLEARNING_EVALUATION_H
#define DEEPREINFORCEMENTLEARNING_EVALUATION_H

#include <string>
#include <iostream>
#include <Game.h>
#include <AI/Ai.h>
#include <AI/NeuralNetAi.h>
#include <TicTacToe/TicTacToeAdapter.h>

struct EvalResult
{
	int wins = 0;
	int losses = 0;
	int draws = 0;
};


class Evaluation
{
public:
	Evaluation(torch::DeviceType device, int mctsCount);

	static EvalResult eval(Ai* neuralNetAi, Ai* miniMaxAi, Game* game, int numberEvalGames = 100);
	static int runGame(Ai* ai1, Ai* ai2, Game* game);

	EvalResult evalMultiThreaded(MultiThreadingNeuralNetManager* threadManager, Ai* miniMaxAi, Game* game, int numberEvalGames = 100);
	void selfPlayMultiThreadGames(MultiThreadingNeuralNetManager* threadManager, Ai* miniMaxAi, 
		Game* game, EvalResult& result, int& gamesToPlay, int& color);
	int runGameMultiThreaded(MultiThreadingNeuralNetManager* threadManager, Ai* minMaxAi, Game* game, int neuralNetColor);

private:
	std::mutex mut;
	int mctsCount = 50;
	torch::DeviceType device = torch::kCUDA;
};


#endif //DEEPREINFORCEMENTLEARNING_EVALUATION_H

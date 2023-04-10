#ifndef DEEPREINFORCEMENTLEARNING_EVALUATION_H
#define DEEPREINFORCEMENTLEARNING_EVALUATION_H

#include <string>
#include <iostream>
#include <Game.h>
#include <Other/Ai.h>
#include <Other/NeuralNetAi.h>
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
	void eval(NeuralNetwork* net, Ai* miniMaxAi, Game* game, int batchSize, EvalResult& outEval, int numberEvalGames);

private:
	std::mutex m_mut;
	int m_gamesToPlay = 0;
	int m_currentColor = 0;
	int m_mctsCount = 50;
	torch::DeviceType m_device = torch::kCPU;
};


#endif //DEEPREINFORCEMENTLEARNING_EVALUATION_H

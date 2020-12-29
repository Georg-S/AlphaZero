#ifndef DEEPREINFORCEMENTLEARNING_EVALUATION_H
#define DEEPREINFORCEMENTLEARNING_EVALUATION_H

#include <string>
#include <iostream>
#include <Game.h>
#include <AI/Ai.h>
#include <AI/NeuralNetAi.h>

struct EvalResult 
{
	int wins;
	int losses;
	int draws;
};


class Evaluation 
{
public:
	static EvalResult eval(Ai* neuralNetAi, Ai* miniMaxAi, Game* game, int numberEvalGames = 100);
	static int runGame(Ai* ai1, Ai* ai2, Game* game);
};


#endif //DEEPREINFORCEMENTLEARNING_EVALUATION_H

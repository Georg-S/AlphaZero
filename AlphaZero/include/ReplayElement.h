#ifndef DEEPREINFORCEMENTLEARNING_REPLAYELEMENT_H
#define DEEPREINFORCEMENTLEARNING_REPLAYELEMENT_H

#include <vector>
#include <tuple>

struct ReplayElement
{
	ReplayElement(std::string state, int currentPlayer, std::vector<std::pair<int,float>> probs, float result)
		: state(std::move(state)),
		currentPlayer(currentPlayer),
		mctsProbabilities(std::move(probs)),
		result(result)
	{
	}

	std::string state;
	int currentPlayer;
	float result;
	std::vector<std::pair<int, float>> mctsProbabilities;
};


#endif //DEEPREINFORCEMENTLEARNING_REPLAYELEMENT_H

#ifndef DEEPREINFORCEMENTLEARNING_REPLAYELEMENT_H
#define DEEPREINFORCEMENTLEARNING_REPLAYELEMENT_H

struct ReplayElement
{
	ReplayElement(std::string state, int currentPlayer, std::vector<float> probs, float result)
		: state(std::move(state)),
		currentPlayer(currentPlayer),
		mctsProbabilities(std::move(probs)),
		result(result)
	{
	}

	std::string state;
	int currentPlayer;
	float result;
	std::vector<float> mctsProbabilities;
};


#endif //DEEPREINFORCEMENTLEARNING_REPLAYELEMENT_H

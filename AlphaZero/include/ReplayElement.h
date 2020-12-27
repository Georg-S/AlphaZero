#ifndef DEEPREINFORCEMENTLEARNING_REPLAYELEMENT_H
#define DEEPREINFORCEMENTLEARNING_REPLAYELEMENT_H

struct ReplayElement 
{
	ReplayElement(std::string state, int currentPlayer, std::vector<float> probs, float result) 
	{
		this->state = state;
		this->currentPlayer = currentPlayer;
		this->mctsProbabilities = probs;
		this->result = result;
	}

	std::string state;
	int currentPlayer;
	float result;
	std::vector<float> mctsProbabilities;
};


#endif //DEEPREINFORCEMENTLEARNING_REPLAYELEMENT_H

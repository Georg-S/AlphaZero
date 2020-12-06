#ifndef DEEPREINFORCEMENTLEARNING_AI_H
#define DEEPREINFORCEMENTLEARNING_AI_H

#include <string>

class Ai {
public:
	virtual int getMove(std::string state, int playerColor) = 0;
};

#endif //DEEPREINFORCEMENTLEARNING_AI_H

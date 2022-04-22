#ifndef DEEPREINFORCEMENTLEARNING_AI_H
#define DEEPREINFORCEMENTLEARNING_AI_H

#include <string>

class Ai
{
public:
	virtual ~Ai() = default;
	virtual int getMove(const std::string& state, int playerColor) = 0;
};

#endif //DEEPREINFORCEMENTLEARNING_AI_H

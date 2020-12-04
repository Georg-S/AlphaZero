#ifndef PROTOTYPE_AI_H
#define PROTOTYPE_AI_H

#include <string>

class Ai{
public:
    virtual int getMove(std::string state, int playerColor) = 0;
};

#endif //PROTOTYPE_AI_H

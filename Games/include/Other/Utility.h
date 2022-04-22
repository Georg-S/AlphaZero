#ifndef DEEPREINFORCEMENTLEARNING_UTILITY_H
#define DEEPREINFORCEMENTLEARNING_UTILITY_H

#include <cstdint>
#include <cassert>
#include <vector>

namespace game 
{
	void setBit(uint32_t& num, int bitIndex);
	void resetLowestBit(uint32_t& num);
	int lowestBitIndex(uint32_t num);
	bool isBitSet(uint32_t num, int bitIndex);
	template <typename T>
	T getRandomElement(const std::vector<T>& vec)
	{
		assert(!vec.empty());
		return vec[rand() % vec.size()];
	}
}

#endif //DEEPREINFORCEMENTLEARNING_UTILITY_H

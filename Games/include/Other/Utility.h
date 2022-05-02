#ifndef DEEPREINFORCEMENTLEARNING_UTILITY_H
#define DEEPREINFORCEMENTLEARNING_UTILITY_H

#include <cstdint>
#include <cassert>
#include <vector>
#include <algorithm>
#include <iostream>

namespace game 
{
	int lowestBitIndex(uint32_t num);
	int lowestBitIndex(uint64_t num);

	template <typename T>
	void setBit(T& num, int index) 
	{
		num |= T(1) << index;
	}
	template <typename T>
	void resetLowestBit(T& num) 
	{
		num &= (num - 1);
	}
	template <typename T>
	bool isBitSet(T num, int index) 
	{
		return num & (T(1) << index);
	}
	template <typename T>
	T getRandomElement(const std::vector<T>& vec)
	{
		assert(!vec.empty());
		return vec[rand() % vec.size()];
	}
	template <typename T>
	void printBitBoard(T num, int width, int height)
	{
		for (int y = height - 1; y >= 0; y--)
		{
			for (int x = 0; x < width; x++)
			{
				std::cout << isBitSet(num, x + y * width) << " ";
			}
			std::cout << std::endl;
		}
	}
	/// Input two vectors, one for the elements and a second for the corresponding values, to get the element(s) with the highest value
	template <typename T, typename Z>
	std::vector<T> getHighestValueElements(const std::vector<T>& elements, const std::vector<Z>& values) 
	{
		assert(!elements.empty());
		assert(elements.size() == values.size());
		const int highestValue = *(std::max_element(values.begin(), values.end()));

		std::vector <T> highestElements;
		for (int i = 0; i < elements.size(); i++)
		{
			if (values[i] == highestValue)
				highestElements.push_back(elements[i]);
		}

		return highestElements;
	}
}

#endif //DEEPREINFORCEMENTLEARNING_UTILITY_H

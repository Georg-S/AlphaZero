#ifndef DEEPREINFORCEMENTLEARNING_ALZ_UTILITY_H
#define DEEPREINFORCEMENTLEARNING_ALZ_UTILITY_H

#include <random>
#include <thread>
#include <cassert>
#include <algorithm>

namespace ALZ
{
	int getRandomNumber(int min, int max);
	double getRandomNumber(double min, double max);
	std::mt19937& getRNG();
	/// Returns an index of the probs vector, weighted by the elements inside the probs vector
	int getRandomIndex(const std::vector<float>& probs, float sumOfProbs);

	template <class T>
	T sum(std::vector<T>& vec) 
	{
		T sum = T(0);
		for (auto n : vec)
			sum += n;

		return sum;
	}

	template <class T>
	int getMaxElementIndex(const std::vector<T>& vec)
	{
		assert(!vec.empty());
		return std::max_element(vec.begin(), vec.end()) - vec.begin();
	}
}

#endif //DEEPREINFORCEMENTLEARNING_ALZ_UTILITY_H

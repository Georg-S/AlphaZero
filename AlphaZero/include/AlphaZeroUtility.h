#ifndef DEEPREINFORCEMENTLEARNING_ALZ_UTILITY_H
#define DEEPREINFORCEMENTLEARNING_ALZ_UTILITY_H

#include <random>
#include <iostream>
#include <thread>
#include <cassert>
#include <algorithm>

namespace ALZ
{
	long long getCurrentTime();
	int getRandomNumber(int min, int max);
	double getRandomNumber(double min, double max);
	std::mt19937& getRNG();
	/// Returns an index of the probs vector, weighted by the elements inside the probs vector
	int getRandomIndex(const std::vector<float>& probs, float sumOfProbs);

	template <class T>
	T sum(const std::vector<T>& vec) 
	{
		auto sum = T(0);
		for (const auto& n : vec)
			sum += n;

		return sum;
	}

	template <typename T>
	T getRandomElement(const std::vector<T>& vec)
	{
		assert(!vec.empty());
		return vec[getRandomNumber(0, vec.size() - 1)];
	}

	template <class T>
	int getMaxElementIndex(const std::vector<T>& vec)
	{
		assert(!vec.empty());
		return std::max_element(vec.begin(), vec.end()) - vec.begin();
	}

	class ScopedTimer
	{
	public:
		ScopedTimer(std::string message = "");
		~ScopedTimer();
	private:
		std::string m_message;
		long long m_startTime;
	};
}

#endif //DEEPREINFORCEMENTLEARNING_ALZ_UTILITY_H

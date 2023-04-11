#ifndef DEEPREINFORCEMENTLEARNING_ALZ_UTILITY_H
#define DEEPREINFORCEMENTLEARNING_ALZ_UTILITY_H

#include <random>
#include <iostream>
#include <thread>
#include <cassert>
#include <algorithm>
#include <map>

namespace ALZ
{
	long long getCurrentTime();
	int getRandomNumber(int min, int max);
	uint64_t getRandomLongNumber(uint64_t min, uint64_t max);
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

	template <class T, class V>
	T sumKey(const std::map<T, V>& map)
	{
		auto sum = T(0);
		for (const auto [key, val] : map)
			sum += key;

		return sum;
	}

	template <class T, class V>
	V sumValue(const std::map<T, V>& map)
	{
		auto sum = V(0);
		for (const auto [key, val] : map)
			sum += val;

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

	/*
	Appends / Moves all elements from the source to the end of the destination Vector
	Leaves the elements of the source vector in a valid but unspecified state
	*/
	template <class T>
	void merge(std::vector<T>& destination, std::vector<T>& source)
	{
		destination.insert(destination.end()
			, std::make_move_iterator(source.begin())
			, std::make_move_iterator(source.end()));
	}

	class ScopedTimer
	{
	public:
		ScopedTimer()
		{
			m_startTime = getCurrentTime();
		}
		~ScopedTimer()
		{
			auto endTime = getCurrentTime();
			const auto timePassed = endTime - m_startTime;
			auto milliseconds = timePassed;
			auto seconds = timePassed / 1000;
			auto minutes = seconds / 60;
			auto hours = minutes / 60;

			auto displayedMilliseconds = timePassed % 1000;
			auto displayedSeconds = seconds % 60;
			auto displayedMinutes = minutes % 60;
			auto displayedHours = hours;

			std::cout << "Time passed: ";
			if (displayedHours > 0)
				std::cout << displayedHours << " hours, ";
			if (displayedMinutes > 0)
				std::cout << displayedMinutes << " minutes, ";
			if (displayedSeconds > 0)
				std::cout << displayedSeconds << " seconds, ";

			std::cout << displayedMilliseconds << " milliseconds" << std::endl;
		}
	private:
		long long m_startTime;
	};
}

#endif //DEEPREINFORCEMENTLEARNING_ALZ_UTILITY_H

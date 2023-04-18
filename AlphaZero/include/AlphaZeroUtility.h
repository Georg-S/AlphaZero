#ifndef DEEPREINFORCEMENTLEARNING_ALZ_UTILITY_H
#define DEEPREINFORCEMENTLEARNING_ALZ_UTILITY_H

#include <string>
#include <random>
#include <iostream>
#include <set>
#include <map>
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
	int getRandomAction(const std::vector<std::pair<int, float>>&);
	int getBestAction(const std::vector<std::pair<int, float>>& probabilities);

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

	template <class T>
	long long memSize(const T& type) 
	{
		return sizeof(T);
	}

	template <class T>
	long long memSize(const std::vector<T>& vec) 
	{
		long long size = sizeof(vec);
		for (const auto& elem : vec) 
			size += memSize(elem);

		return size;
	}

	template <class T>
	long long memSize(const std::set<T>& set)
	{
		long long size = sizeof(set);
		for (const auto& elem : set)
			size += memSize(elem);

		return size;
	}

	template <class T>
	long long memSize(const std::string& str)
	{
		long long size = sizeof(std::string);
		for (char c: str)
			size += memSize(c);

		return size;
	}

	template <class T, class Y>
	long long memSize(const std::map<T,Y>& map)
	{
		long long size = sizeof(std::map<T, Y>);
		for (const auto& [key, value] : map) 
		{
			size += memSize(key);
			size += memSize(value);
		}

		return size;
	}

	class ScopedTimer
	{
	public:
		ScopedTimer()
		{
			m_startTime = getCurrentTime();
		}

		ScopedTimer(std::string headerMessage) 
			: m_headerMessage(std::move(headerMessage))
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

			if (m_headerMessage.empty())
				std::cout << "Time passed: ";
			else
				std::cout << m_headerMessage;

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
		std::string m_headerMessage;
	};
}

#endif //DEEPREINFORCEMENTLEARNING_ALZ_UTILITY_H

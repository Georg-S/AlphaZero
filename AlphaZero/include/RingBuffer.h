#ifndef DEEPREINFORCEMENTLEARNING_RINGBUFFER_H
#define DEEPREINFORCEMENTLEARNING_RINGBUFFER_H

#include <vector>
#include <algorithm>
#include <iterator>
#include <random>
#include "AlphaZeroUtility.h"

template <class T>
class RingBuffer
{
public:
	RingBuffer() = default;
	RingBuffer(int maxSize)
	{
		m_maxSize = maxSize;
		m_data.reserve(maxSize);
	}

	T& operator[](size_t index)
	{
		return m_data[index];
	}

	void add(T&& element)
	{
		assert(m_maxSize > 0);
		if (m_currentIndex == m_maxSize)
		{
			m_wrappedAround = true;
			m_currentIndex = 0;
		}

		if (!m_wrappedAround)
			m_data.emplace_back(std::move(element));
		else
			m_data[m_currentIndex] = std::move(element);
		m_currentIndex++;
	}

	void add(const T& element)
	{
		add(T(element));
	}

	int size() const
	{
		return m_data.size();
	}

	int getMaxSize() const
	{
		return m_maxSize;
	}

	void add(std::vector<T>&& elements) 
	{
		for (T& elem : elements)
			add(std::move(elem));
	}

	void add(const std::vector<T>& elements)
	{
		for (T elem : elements)
			add(std::move(elem));
	}

	void clear()
	{
		m_data.clear();
		m_currentIndex = 0;
		m_wrappedAround = false;
	}

	std::vector<T> getRandomSample(int sampleSize) const
	{
		std::vector<T> destination;
		std::sample(m_data.begin(), m_data.end(), std::back_inserter(destination), sampleSize, ALZ::getRNG());
		return destination;
	}

	double memSize() const 
	{
		double result = 0;
		for (const auto& elem : m_data)
			result += ALZ::memSize(m_data);

		return result / ALZ::GigaByte;
	}

private:
	int m_maxSize = 0;
	int m_currentIndex = 0;
	bool m_wrappedAround = false;
	std::vector<T> m_data;
};

#endif //DEEPREINFORCEMENTLEARNING_RINGBUFFER_H

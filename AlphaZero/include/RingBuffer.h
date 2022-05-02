#ifndef DEEPREINFORCEMENTLEARNING_RINGBUFFER_H
#define DEEPREINFORCEMENTLEARNING_RINGBUFFER_H

#include <vector>
#include <algorithm>
#include <iterator>
#include <random>

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

	T& operator[](int index)
	{
		return m_data[index];
	}

	void add_m(T&& element) 
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

	void add(T element)
	{
		add_m(std::move(element));
	}

	int size() const
	{
		return m_data.size();
	}

	int getMaxSize() const
	{
		return m_maxSize;
	}

	void add(const std::vector<T>& elements)
	{
		for (T elem : elements)
			add_m(std::move(elem));
	}

	void clear()
	{
		m_data.clear();
		m_currentIndex = 0;
		m_wrappedAround = false;
	}

	std::vector<T> getRandomSample(int sampleSize) 
	{
		std::vector<T> destination;
		std::sample(m_data.begin(), m_data.end(), std::back_inserter(destination), sampleSize, std::mt19937{ std::random_device{}() });
		return destination;
	}

private:
	int m_maxSize = 0;
	int m_currentIndex = 0;
	bool m_wrappedAround = false;
	std::vector<T> m_data;
};

#endif //DEEPREINFORCEMENTLEARNING_RINGBUFFER_H

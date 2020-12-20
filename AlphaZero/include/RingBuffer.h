#ifndef DEEPREINFORCEMENTLEARNING_RINGBUFFER_H
#define DEEPREINFORCEMENTLEARNING_RINGBUFFER_H


#include <vector>
#include <algorithm>
#include <iterator>
#include <random>

template <class T>
class RingBuffer {

public:
	RingBuffer() {}

	RingBuffer(int maxSize) {
		this->maxSize = maxSize;
	}


	T& operator[](int index) {
		return data[index];
	}

	void add(const T& element) {
		if (currentIndex == maxSize) {
			wrappedAround = true;
			currentIndex = 0;
		}

		if (!wrappedAround)
			data.push_back(element);
		else
			data[currentIndex] = element;
		currentIndex++;
	}

	int size() const {
		return data.size();
	}

	int getMaxSize() const {
		return maxSize;
	}

	void add(const std::vector<T>& elements) {
		for (int x = 0; x < elements.size(); x++)
			add(elements[x]);
	}

	void clear() {
		data.clear();
		currentIndex = 0;
		wrappedAround = false;
	}

	std::vector<T> convertToVec() const {
		return data;
	}

	void getRandomSample(int sampleSize, std::vector<T>& destination) const {
		std::sample(data.begin(), data.end(), std::back_inserter(destination), sampleSize, std::mt19937{ std::random_device{}() });
	}

private:
	int maxSize;
	int currentIndex = 0;
	bool wrappedAround = false;
	std::vector<T> data;
};




#endif //DEEPREINFORCEMENTLEARNING_RINGBUFFER_H

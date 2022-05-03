#include "Utility.h"

//TODO is this good and valid?
static std::mt19937 createRNG()
{
	auto id = std::this_thread::get_id();
	std::hash<std::thread::id> hasher;

	auto seed = hasher(id);
	seed += time(NULL);
	return std::mt19937(seed);
}

//TODO is this good and valid?
thread_local std::mt19937 generator = createRNG();

int ALZ::getRandomNumber(int min, int max)
{
	std::uniform_int_distribution<int> distribution(min, max);

	return distribution(generator);
}

double ALZ::getRandomNumber(double min, double max)
{
	std::uniform_real_distribution<double> distribution(min, max);
	
	return distribution(generator);
}

std::mt19937& ALZ::getRNG()
{
	return generator;
}

int ALZ::getRandomIndex(const std::vector<float>& probs, float sumOfProbs)
{
	assert(!probs.empty());
	double randomNum = getRandomNumber(0.0, sumOfProbs);

	double accumulate = 0.f;
	for(int i = 0; i < probs.size(); i++)
	{
		accumulate += probs[i];
		if (accumulate >= randomNum)
			return i;
	}

	return probs.size() - 1;
}


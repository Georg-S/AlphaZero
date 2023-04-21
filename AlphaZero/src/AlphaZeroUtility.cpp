#include "AlphaZeroUtility.h"

static std::mt19937 createRNG()
{
	auto id = std::this_thread::get_id();
	std::hash<std::thread::id> hasher;

	auto seed = hasher(id);
	seed += time(NULL);

	return std::mt19937(seed);
}

thread_local std::mt19937 generator = createRNG();

long long ALZ::getCurrentTime()
{
	auto current_time = std::chrono::system_clock::now();
	return std::chrono::time_point_cast<std::chrono::milliseconds>(current_time).time_since_epoch().count();
}

int ALZ::getRandomNumber(int min, int max)
{
	std::uniform_int_distribution<int> distribution(min, max);

	return distribution(generator);
}

uint64_t ALZ::getRandomLongNumber(uint64_t min, uint64_t max)
{
	std::uniform_int_distribution<uint64_t> distribution(min, max);

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

int ALZ::getRandomAction(const std::vector<std::pair<int, float>>& probs)
{
	assert(!probs.empty());
	float sumOfProbs = 0.0;
	for (const auto& [action, prob] : probs)
		sumOfProbs += prob;

	double randomNum = getRandomNumber(0.0, sumOfProbs);

	double accumulate = 0.f;
	for (const auto& [action, prob] : probs)
	{
		accumulate += prob;
		if (accumulate >= randomNum)
			return action;
	}

	return probs.size() - 1;
}

int ALZ::getBestAction(const std::vector<std::pair<int, float>>& probabilities)
{
	float bestProb = -INFINITY;
	int bestAction = INT_MAX;
	for (const auto& [action, prob] : probabilities)
	{
		if (prob > bestProb)
		{
			bestProb = prob;
			bestAction = action;
		}
	}

	return bestAction;
}

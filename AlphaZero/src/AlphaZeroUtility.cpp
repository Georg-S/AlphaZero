#include "AlphaZeroUtility.h"

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

ALZ::ScopedTimer::ScopedTimer(std::string message)
{
	if (m_message.empty())
		m_message = "Time passed in ms: ";
	else
		m_message = std::move(message);

	m_startTime = getCurrentTime();
}

ALZ::ScopedTimer::~ScopedTimer()
{
	auto endTime = getCurrentTime();

	std::cout << m_message << endTime - m_startTime << std::endl;
}

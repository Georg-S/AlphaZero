#include <gtest/gtest.h>
#include <stddef.h>
#include <stdio.h>
#include <algorithm>
#include <chrono>
#include <random>
#include <ratio>
#include <omp.h>
#include <vector>
#include <AlphaZeroUtility.h>
#include <NeuralNetworks/DefaultNeuralNet.h>
#include <TicTacToe/TicTacToeAdapter.h>
#include "TestConfig.h"

#if RunTests

TEST(MiscellaneousTest, test_get_random_element)
{
	std::vector<int> vec = { 3 };
	ASSERT_EQ(ALZ::getRandomElement(vec), 3);
}

TEST(MiscellaneousTest, test_sum_vector)
{
	std::vector<int> test{ 0,1,2,3,4,5,6,7,8,9 };
	int sum = ALZ::sum(test);

	ASSERT_EQ(sum, 45);
}

TEST(MiscellaneousTest, test_sum_vector_0_size_vector)
{
	std::vector<int> test;
	int sum = ALZ::sum(test);

	ASSERT_FLOAT_EQ(sum, 0);
}

TEST(MiscellaneousTest, test_random_action)
{
	srand(time(NULL));
	const int amount = 1000000;
	std::vector<std::pair<int, float>> test{ {0, 0.f}, {1, 0.2f}, {2, 0.1f}, {3, 0.5}, {4, 0.1}, {5, 0.1}, {6, 0.0}, {7, 0.0}, {8, 0.0} };
	std::vector<int> results = std::vector<int>(9, 0);

	for (int i = 0; i < amount; i++)
	{
		int buf = ALZ::getRandomAction(test);
		results[buf] += 1;
	}

	EXPECT_NEAR(results[3], amount / 2, amount * 0.01);
}

#endif //RunTests


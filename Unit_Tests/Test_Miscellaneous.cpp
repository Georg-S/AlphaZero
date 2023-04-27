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
#include <MonteCarloTreeSearch.h>
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

#endif //RunTests


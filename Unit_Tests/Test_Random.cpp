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

TEST(RandomTest, test_get_random_element) 
{
	std::vector<int> vec = { 3 };
	ASSERT_EQ(ALZ::getRandomElement(vec), 3);
}

#endif //RunTests


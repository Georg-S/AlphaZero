#include <gtest/gtest.h>
#include <Training.h>
#include <iostream>
#include <TicTacToe/TicTacToeAdapter.h>


TEST(Training, test_random_action)
{
	srand(time(NULL));
	const int amount = 1000000;
	std::vector<float> test{ 0.f, 0.2f, 0.1f, 0.5, 0.1, 0.1, 0.0, 0.0, 0.0 };
	std::vector<int> results = std::vector<int>(9, 0);

	for (int i = 0; i < amount; i++) {
		int buf = Training::getRandomAction(test);
		results[buf] += 1;
	}

	EXPECT_NEAR(results[3], amount / 2, amount * 0.01);
}
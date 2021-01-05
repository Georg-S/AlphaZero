#include <gtest/gtest.h>
#include <stddef.h>
#include <torch/torch.h>
#include <stdio.h>
#include <algorithm>
#include <chrono>
#include <random>
#include <ratio>
#include <omp.h>
#include <vector>

/*
TEST(RandomTest, parallel) {
	#pragma omp parallel for
	for(int i=0;i<10;i++){
		std::cout << i << std::endl;
	}
}

TEST(RandomTest, torchConcate)
{
	auto tens1 = torch::tensor({});
	auto tens2 = torch::unsqueeze(torch::zeros({ 3, 3, 9 }), 0);

	auto resultTens = torch::cat({ tens1, tens2 });

	std::cout << resultTens << std::endl;
}
*/
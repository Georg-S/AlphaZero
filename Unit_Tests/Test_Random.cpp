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
#include <MonteCarloTreeSearch.h>
#include <NeuralNetworks/DefaultNeuralNet.h>
#include <TicTacToe/TicTacToeAdapter.h>

/*
TEST(RandomTest, parallel) {
	#pragma omp parallel for
	for(int i=0;i<10;i++){
		std::cout << i << std::endl;
	}
}
*/
/*
TEST(RandomTest, test_performance_mcts)
{
	torch::DeviceType device = torch::kCUDA;
	DefaultNeuralNet net(2, 3, 3, 9, device);
	TicTacToeAdapter adap = TicTacToeAdapter();
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(9);
	std::string state = adap.getInitialGameState();

	std::chrono::steady_clock::time_point startTime, endTime;
	mcts.search(2, state, &net, &adap, 2, device);
	mcts.clearAll();

	//Single
	startTime = std::chrono::high_resolution_clock::now();
	mcts.search(90, state, &net, &adap, 2, device);
	endTime = std::chrono::high_resolution_clock::now();
	double singleTime = ((std::chrono::duration<double>)(endTime - startTime)).count();

	mcts.clearAll();

	//Batch
	startTime = std::chrono::high_resolution_clock::now();
	mcts.search(10, 9, state, &net, &adap, 2, device);
	endTime = std::chrono::high_resolution_clock::now();
	double batchTime = ((std::chrono::duration<double>)(endTime - startTime)).count();

	mcts.clearAll();


	std::cout << "Single Time: " << singleTime << " Batch Time: " << batchTime << std::endl;

	ASSERT_GT(singleTime, batchTime);
}
*/

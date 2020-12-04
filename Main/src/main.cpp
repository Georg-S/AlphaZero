#include <iostream>
#include <string>
#include <chrono>
#include "GameHandling/ChessHandler.h"
#include "GameHandling/ConnectFourHandler.h"
#include "GameHandling/TicTacToeHandler.h"
#include <QApplication>
#include "UI/MainWindow.h"
#include <omp.h>
#include <AlphaZero/NeuralNetworks/DefaultNoResidual.h>
#include <utility>
#include <chrono>

void initRNGS(){
    srand(time(NULL));
    std::mt19937 mt_rand(time(0));
}

std::vector<torch::Tensor> generateData(int count, torch::DeviceType device) {
	std::vector<torch::Tensor> data;

	for (int i = 0; i < count; i++)
		data.push_back(torch::rand({1,2,3,3},device));

	return data;
}

double calculateNetOutputSingle(int count) {
	torch::DeviceType device = torch::kCUDA;
	DefaultNoResidual net = DefaultNoResidual(2, 3, 3, 9, device);
	auto input = generateData(count, device);

	auto startTime = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < count; i++)
		auto netPredict = net.calculate(input[i]);

	auto endTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = endTime - startTime;
	
	return elapsed.count();
}

double calculateNetOutputTogether(int count) {
	torch::DeviceType device = torch::kCUDA;
	DefaultNoResidual net = DefaultNoResidual(2, 3, 3, 9, device);

	auto startTime = std::chrono::high_resolution_clock::now();

	torch::Tensor example = torch::rand({ count,2,3,3 }, device);
	auto netPredict = net.calculate(example);

	auto endTime = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> elapsed = endTime - startTime;
	
	return elapsed.count();
}

#ifdef _WIN32
#undef main
#endif

int main(int argc, char* argv[]) {
    initRNGS();

	int count = 60;
	double singleTime = calculateNetOutputSingle(count);
	double togetherTime = calculateNetOutputTogether(count);

	for (int i = 0; i < 10; i++) {
		singleTime = calculateNetOutputSingle(count);
		togetherTime = calculateNetOutputTogether(count);
	}

	std::cout << "Count: " << count << " Single Time: " << singleTime << " Together Time" << togetherTime << std::endl;

	/*
    QApplication app(argc, argv);
    MainWindow w;
    w.show();

    return app.exec();
	*/
}
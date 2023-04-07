#include <gtest/gtest.h>
#include <chrono>
#include <NeuralNetworks/CutDownAlphaGoZeroNet.h>
#include <NeuralNetworks/DefaultNeuralNet.h>
#include "TestConfig.h"

#if RunTests

TEST(NeuralNet, net_returns_same_value_for_single_element_and_batch)
{
	constexpr int filterCount = 2;

	torch::Tensor inputBatch = torch::rand({ 2,filterCount, 3,3 });
	inputBatch[1] = torch::zeros({ filterCount,3,3 });
	torch::Tensor inputSingleElement = torch::zeros({ 1, filterCount, 3,3 });
	inputSingleElement[0] = inputBatch[0];

	DefaultNeuralNet net = DefaultNeuralNet(filterCount, 3, 3, 9, torch::kCPU);
	net.setToEval();

	auto result = net.calculate(inputSingleElement);
	torch::Tensor resultTens = std::get<1>(result);
	float singleValue1 = *(resultTens[0][0].data_ptr<float>());

	result = net.calculate(inputBatch);
	resultTens = std::get<1>(result);
	float batchValue1 = *(resultTens[0][0].data_ptr<float>());

	ASSERT_FLOAT_EQ(singleValue1, batchValue1);
}

TEST(NeuralNet, net_returns_same_value_after_save_and_load)
{
	torch::Tensor input = torch::rand({ 1,1,3,3 });

	CutDownAlphaGoZeroNet net = CutDownAlphaGoZeroNet(1, 3, 3, 9);
	auto result = net->forward(input);
	torch::Tensor resultTens = std::get<1>(result);
	float value1 = *(resultTens[0][0].data_ptr<float>());

	torch::save(net, "unit_test_net");

	CutDownAlphaGoZeroNet net2 = CutDownAlphaGoZeroNet(1, 3, 3, 9);
	torch::load(net2, "unit_test_net");
	auto result2 = net2->forward(input);
	torch::Tensor resultTens2 = std::get<1>(result2);
	float value2 = *(resultTens2[0][0].data_ptr<float>());

	ASSERT_EQ(value1, value2);
}

#endif RunTests
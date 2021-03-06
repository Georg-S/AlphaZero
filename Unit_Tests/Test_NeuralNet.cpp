#include <gtest/gtest.h>
#include <chrono>
#include <torch/torch.h>
#include <NeuralNetworks/CutDownAlphaGoZeroNet.h>

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
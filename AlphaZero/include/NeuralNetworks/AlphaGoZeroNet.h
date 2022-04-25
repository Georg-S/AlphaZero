#ifndef DEEPREINFORCEMENTLEARNING_ALPHAGOZERONET_H
#define DEEPREINFORCEMENTLEARNING_ALPHAGOZERONET_H

#include <tuple>
// Libtorch has many warnings which clutter the output, so we ignore them
#pragma warning(push, 0)
#include <torch/nn.h>
#pragma warning(pop)
#include "NeuralNetworks/Components/ConvolutionBlock.h"
#include "NeuralNetworks/Components/ResidualBlock.h"
#include "NeuralNetworks/Components/PolicyHead.h"
#include "NeuralNetworks/Components/ValueHead.h"

//The 20 Block AlphaGo Zero Neural Network, as described in the paper: "Mastering the game of Go without human knowledge"
struct AlphaGoZeroNetImpl : public torch::nn::Module
{
	AlphaGoZeroNetImpl(int64_t numPlanes, int64_t width, int64_t height, int64_t numOutputs)
	{
		static const int64_t numFilters = 256;
		static const int64_t valueFilters = 1;
		static const int64_t policyFilters = 2;

		convBlock = register_module("convBlock", ConvolutionBlock(numPlanes, numFilters));

		resBlock1 = register_module("resBlock1", ResidualBlock(numFilters, numFilters, numFilters));
		resBlock2 = register_module("resBlock2", ResidualBlock(numFilters, numFilters, numFilters));
		resBlock3 = register_module("resBlock3", ResidualBlock(numFilters, numFilters, numFilters));
		resBlock4 = register_module("resBlock4", ResidualBlock(numFilters, numFilters, numFilters));
		resBlock5 = register_module("resBlock5", ResidualBlock(numFilters, numFilters, numFilters));
		resBlock6 = register_module("resBlock6", ResidualBlock(numFilters, numFilters, numFilters));
		resBlock7 = register_module("resBlock7", ResidualBlock(numFilters, numFilters, numFilters));
		resBlock8 = register_module("resBlock8", ResidualBlock(numFilters, numFilters, numFilters));
		resBlock9 = register_module("resBlock9", ResidualBlock(numFilters, numFilters, numFilters));
		resBlock10 = register_module("resBlock10", ResidualBlock(numFilters, numFilters, numFilters));
		resBlock11 = register_module("resBlock11", ResidualBlock(numFilters, numFilters, numFilters));
		resBlock12 = register_module("resBlock12", ResidualBlock(numFilters, numFilters, numFilters));
		resBlock13 = register_module("resBlock13", ResidualBlock(numFilters, numFilters, numFilters));
		resBlock14 = register_module("resBlock14", ResidualBlock(numFilters, numFilters, numFilters));
		resBlock15 = register_module("resBlock15", ResidualBlock(numFilters, numFilters, numFilters));
		resBlock16 = register_module("resBlock16", ResidualBlock(numFilters, numFilters, numFilters));
		resBlock17 = register_module("resBlock17", ResidualBlock(numFilters, numFilters, numFilters));
		resBlock18 = register_module("resBlock18", ResidualBlock(numFilters, numFilters, numFilters));
		resBlock19 = register_module("resBlock19", ResidualBlock(numFilters, numFilters, numFilters));



		valueHead = register_module("valueHead", ValueHead(numFilters, valueFilters, width, height));
		policyHead = register_module("policyHead", PolicyHead(numFilters, policyFilters, width, height, numOutputs));
	}

	std::tuple<torch::Tensor, torch::Tensor> forward(const torch::Tensor& input)
	{
		auto buf = convBlock(input);
		buf = resBlock2(resBlock1(buf));
		buf = resBlock4(resBlock3(buf));
		buf = resBlock6(resBlock5(buf));
		buf = resBlock8(resBlock7(buf));
		buf = resBlock10(resBlock9(buf));
		buf = resBlock12(resBlock11(buf));
		buf = resBlock14(resBlock13(buf));
		buf = resBlock16(resBlock15(buf));
		buf = resBlock18(resBlock17(buf));
		buf = resBlock19(buf);

		auto value = valueHead(buf);
		auto policy = policyHead(buf);

		return std::make_tuple(value, policy);
	}

	ConvolutionBlock convBlock = nullptr;
	ResidualBlock resBlock1 = nullptr;
	ResidualBlock resBlock2 = nullptr;
	ResidualBlock resBlock3 = nullptr;
	ResidualBlock resBlock4 = nullptr;
	ResidualBlock resBlock5 = nullptr;
	ResidualBlock resBlock6 = nullptr;
	ResidualBlock resBlock7 = nullptr;
	ResidualBlock resBlock8 = nullptr;
	ResidualBlock resBlock9 = nullptr;
	ResidualBlock resBlock10 = nullptr;
	ResidualBlock resBlock11 = nullptr;
	ResidualBlock resBlock12 = nullptr;
	ResidualBlock resBlock13 = nullptr;
	ResidualBlock resBlock14 = nullptr;
	ResidualBlock resBlock15 = nullptr;
	ResidualBlock resBlock16 = nullptr;
	ResidualBlock resBlock17 = nullptr;
	ResidualBlock resBlock18 = nullptr;
	ResidualBlock resBlock19 = nullptr;

	ValueHead valueHead = nullptr;
	PolicyHead policyHead = nullptr;
};
TORCH_MODULE(AlphaGoZeroNet);


#endif //DEEPREINFORCEMENTLEARNING_ALPHAGOZERONET_H
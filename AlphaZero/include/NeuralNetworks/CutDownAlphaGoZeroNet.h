#ifndef DEEPREINFORCEMENTLEARNING_CUTDOWNALPHAGOZERONET_H
#define DEEPREINFORCEMENTLEARNING_CUTDOWNALPHAGOZERONET_H

#include <torch/nn.h>
#include <tuple>
#include "NeuralNetworks/Components/ConvolutionBlock.h"
#include "NeuralNetworks/Components/ResidualBlock.h"
#include "NeuralNetworks/Components/PolicyHead.h"
#include "NeuralNetworks/Components/ValueHead.h"

// A cut down version of the AlphaGoZeroNet  (less residual blocks)
struct CutDownAlphaGoZeroNetImpl : public torch::nn::Module
{
	CutDownAlphaGoZeroNetImpl(int64_t numPlanes, int64_t width, int64_t height, int64_t numOutputs)
	{
		static const int64_t valueFilters = 1;
		static const int64_t policyFilters = 2;

		convBlock = register_module("convBlock", ConvolutionBlock(numPlanes, numFilters));

		resBlock1 = register_module("resBlock1", ResidualBlock(numFilters, numFilters, numFilters));
		resBlock2 = register_module("resBlock2", ResidualBlock(numFilters, numFilters, numFilters));
		resBlock3 = register_module("resBlock3", ResidualBlock(numFilters, numFilters, numFilters));
		resBlock4 = register_module("resBlock4", ResidualBlock(numFilters, numFilters, numFilters));
		resBlock5 = register_module("resBlock5", ResidualBlock(numFilters, numFilters, numFilters));

		valueHead = register_module("valueHead", ValueHead(numFilters, valueFilters, width, height));
		policyHead = register_module("policyHead", PolicyHead(numFilters, policyFilters, width, height, numOutputs));
	}

	std::tuple<torch::Tensor, torch::Tensor> forward(const torch::Tensor& input)
	{
		auto buf = convBlock(input);
		buf = resBlock2(resBlock1(buf));
		buf = resBlock4(resBlock3(buf));
		buf = resBlock5(buf);

		auto value = valueHead(buf);
		auto policy = policyHead(buf);

		return std::make_tuple(value, policy);
	}

	int64_t numFilters = 256;

	ConvolutionBlock convBlock = nullptr;
	ResidualBlock resBlock1 = nullptr;
	ResidualBlock resBlock2 = nullptr;
	ResidualBlock resBlock3 = nullptr;
	ResidualBlock resBlock4 = nullptr;
	ResidualBlock resBlock5 = nullptr;

	ValueHead valueHead = nullptr;
	PolicyHead policyHead = nullptr;
};
TORCH_MODULE(CutDownAlphaGoZeroNet);

#endif //DEEPREINFORCEMENTLEARNING_CUTDOWNALPHAGOZERONET_H

#ifndef DEEPREINFORCEMENTLEARNING_CONVOLUTIONBLOCK_H
#define DEEPREINFORCEMENTLEARNING_CONVOLUTIONBLOCK_H

// Libtorch has many warnings which clutter the output, so we ignore them
#pragma warning(push, 0)
#include <torch/nn.h>
#pragma warning(pop)

struct ConvolutionBlockImpl : public torch::nn::Module
{
	ConvolutionBlockImpl(int64_t inFilters, int64_t outFilters)
	{
		convBody1 = register_module("convBody1", torch::nn::Conv2d(torch::nn::Conv2dOptions(inFilters, outFilters, 3).padding(1).stride(1)));
		normBody1 = register_module("normBody1", torch::nn::BatchNorm2d(outFilters));
	}

	torch::Tensor forward(const torch::Tensor& input)
	{
		return torch::relu(normBody1(convBody1(input)));
	}

	torch::nn::Conv2d convBody1 = nullptr;
	torch::nn::BatchNorm2d normBody1 = nullptr;
};
TORCH_MODULE(ConvolutionBlock);

#endif // DEEPREINFORCEMENTLEARNING_CONVOLUTIONBLOCK_H
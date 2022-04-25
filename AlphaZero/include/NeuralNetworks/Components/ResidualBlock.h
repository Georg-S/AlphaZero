#ifndef DEEPREINFORCEMENTLEARNING_RESIDUALBLOCK_H
#define DEEPREINFORCEMENTLEARNING_RESIDUALBLOCK_H

// Libtorch has many warnings which clutter the output, so we ignore them
#pragma warning(push, 0)
#include <torch/nn.h>
#pragma warning(pop)

struct ResidualBlockImpl : public torch::nn::Module
{
	ResidualBlockImpl(int64_t inFilters, int64_t numFilters, int64_t outFilters)
	{
		convBody1 = register_module("convBody1", torch::nn::Conv2d(torch::nn::Conv2dOptions(inFilters, numFilters, 3).padding(1).stride(1)));
		normBody1 = register_module("normBody1", torch::nn::BatchNorm2d(numFilters));

		convBody2 = register_module("convBody2", torch::nn::Conv2d(torch::nn::Conv2dOptions(numFilters, outFilters, 3).padding(1).stride(1)));
		normBody2 = register_module("normBody2", torch::nn::BatchNorm2d(outFilters));
	}

	torch::Tensor forward(const torch::Tensor& input)
	{
		torch::Tensor x = input;
		x = torch::relu(normBody1(convBody1(input)));
		x = normBody2(convBody2(x));
		x = torch::relu(x + input);

		return x;
	}

	torch::nn::Conv2d convBody1 = nullptr;
	torch::nn::Conv2d convBody2 = nullptr;

	torch::nn::BatchNorm2d normBody1 = nullptr;
	torch::nn::BatchNorm2d normBody2 = nullptr;
};
TORCH_MODULE(ResidualBlock);

#endif //DEEPREINFORCEMENTLEARNING_RESIDUALBLOCK_H
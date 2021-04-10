#ifndef DEEPREINFORCEMENTLEARNING_VALUEHEAD_H
#define DEEPREINFORCEMENTLEARNING_VALUEHEAD_H

#include <torch/nn.h>

struct ValueHeadImpl : public torch::nn::Module
{
	ValueHeadImpl(int64_t inFilters, int64_t valueFilters, int64_t width, int64_t height)
	{
		int64_t valueLinearSize = width * height * valueFilters;

		convValue = register_module("convValue", torch::nn::Conv2d(torch::nn::Conv2dOptions(inFilters, valueFilters, 1).stride(1)));
		normValue = register_module("normValue", torch::nn::BatchNorm2d(valueFilters));

		linearValue1 = register_module("linearValue1", torch::nn::Linear(valueLinearSize, linearNeurons));
		linearValue2 = register_module("linearValue2", torch::nn::Linear(linearNeurons, 1));
	}

	torch::Tensor forward(const torch::Tensor& input)
	{
		int64_t batchSize = input.size(0);

		torch::Tensor value = torch::relu(normValue(convValue(input)));
		value = value.view({ batchSize, -1 });
		value = torch::relu(linearValue1(value));
		value = torch::tanh(linearValue2(value));

		return value;
	}

private:
	int64_t linearNeurons = 256;

	torch::nn::Conv2d convValue = nullptr;
	torch::nn::BatchNorm2d normValue = nullptr;
	torch::nn::Linear linearValue1 = nullptr;
	torch::nn::Linear linearValue2 = nullptr;
};
TORCH_MODULE(ValueHead);


#endif //DEEPREINFORCEMENTLEARNING_VALUEHEAD_H

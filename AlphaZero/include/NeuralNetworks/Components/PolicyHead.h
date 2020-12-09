#ifndef DEEPREINFORCEMENTLEARNING_POLICYHEAD_H
#define DEEPREINFORCEMENTLEARNING_POLICYHEAD_H

#include <torch/nn.h>

struct PolicyHeadImpl : public torch::nn::Module
{
	PolicyHeadImpl(int64_t inFilters, int64_t policyFilters, int64_t width, int64_t height, int64_t outputs)
	{
		int64_t policyLinearSize = width * height * policyFilters;

		convPolicy = register_module("convPolicy", torch::nn::Conv2d(torch::nn::Conv2dOptions(inFilters, policyFilters, 1)));
		normPolicy = register_module("normPolicy", torch::nn::BatchNorm2d(policyFilters));
		linearPolicy = register_module("linearPolicy", torch::nn::Linear(policyLinearSize, outputs));
	}

	torch::Tensor forward(const torch::Tensor& input)
	{
		int64_t batchSize = input.size(0);

		auto pol = torch::relu(normPolicy(convPolicy(input)));
		pol = pol.view({ batchSize, -1 });
		pol = linearPolicy(pol);
		pol = torch::softmax(pol, 1);

		return pol;
	}


private:
	torch::nn::Conv2d convPolicy = nullptr;
	torch::nn::BatchNorm2d normPolicy = nullptr;
	torch::nn::Linear linearPolicy = nullptr;

};
TORCH_MODULE(PolicyHead);


#endif //DEEPREINFORCEMENTLEARNING_POLICYHEAD_H
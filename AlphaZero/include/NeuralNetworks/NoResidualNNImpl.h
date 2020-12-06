#ifndef DEEPREINFORCEMENTLEARNING_NORESIDUALNNIMPL_H
#define DEEPREINFORCEMENTLEARNING_NORESIDUALNNIMPL_H

#include <torch/torch.h>
#include <torch/nn.h>
#include <torch/nn/pimpl.h>
#include <torch/ordered_dict.h>
#include <torch/serialize/archive.h>
#include <torch/types.h>
#include <tuple>

struct NoResidualNNImpl : public torch::nn::Module {
	NoResidualNNImpl(int64_t numPlanes, int64_t width, int64_t height, int64_t numOutputs) {
		init(numPlanes, width, height, numOutputs);
	}

	NoResidualNNImpl(int64_t numPlanes, int64_t width, int64_t height, int64_t numOutputs, int64_t numFilters) {
		this->numFilters = numFilters;
		init(numPlanes, width, height, numOutputs);
	}

	void init(int64_t numPlanes, int64_t width, int64_t height, int64_t numOutputs) {
		int64_t  valueFilters = 1;
		int64_t policyFilters = 2;

		int64_t valueLinearSize = width * height * valueFilters;
		int64_t policyLinearSize = width * height * policyFilters;
		this->width = width;
		this->height = height;

		convBody1 = register_module("convBody1", torch::nn::Conv2d(torch::nn::Conv2dOptions(numPlanes, numFilters, 3).padding(1).stride(1)));
		normBody1 = register_module("normBody1", torch::nn::BatchNorm2d(numFilters));

		convBody2 = register_module("convBody2", torch::nn::Conv2d(torch::nn::Conv2dOptions(numFilters, numFilters, 3).padding(1).stride(1)));
		normBody2 = register_module("normBody2", torch::nn::BatchNorm2d(numFilters));

		convBody3 = register_module("convBody3", torch::nn::Conv2d(torch::nn::Conv2dOptions(numFilters, numFilters, 3).padding(1).stride(1)));
		normBody3 = register_module("normBody3", torch::nn::BatchNorm2d(numFilters));

		convBody4 = register_module("convBody4", torch::nn::Conv2d(torch::nn::Conv2dOptions(numFilters, numFilters, 3).padding(1).stride(1)));
		normBody4 = register_module("normBody4", torch::nn::BatchNorm2d(numFilters));

		convBody5 = register_module("convBody5", torch::nn::Conv2d(torch::nn::Conv2dOptions(numFilters, numFilters, 3).padding(1).stride(1)));
		normBody5 = register_module("normBody5", torch::nn::BatchNorm2d(numFilters));

		convBody6 = register_module("convBody6", torch::nn::Conv2d(torch::nn::Conv2dOptions(numFilters, numFilters, 3).padding(1)));
		normBody6 = register_module("normBody6", torch::nn::BatchNorm2d(numFilters));

		convBody7 = register_module("convBody7", torch::nn::Conv2d(torch::nn::Conv2dOptions(numFilters, numFilters, 3).padding(1)));
		normBody7 = register_module("normBody7", torch::nn::BatchNorm2d(numFilters));

		convBody8 = register_module("convBody8", torch::nn::Conv2d(torch::nn::Conv2dOptions(numFilters, numFilters, 3).padding(1)));
		normBody8 = register_module("normBody8", torch::nn::BatchNorm2d(numFilters));

		convBody9 = register_module("convBody9", torch::nn::Conv2d(torch::nn::Conv2dOptions(numFilters, numFilters, 3).padding(1)));
		normBody9 = register_module("normBody9", torch::nn::BatchNorm2d(numFilters));

		convBody10 = register_module("convBody10", torch::nn::Conv2d(torch::nn::Conv2dOptions(numFilters, numFilters, 3).padding(1)));
		normBody10 = register_module("normBody10", torch::nn::BatchNorm2d(numFilters));

		convBody11 = register_module("convBody11", torch::nn::Conv2d(torch::nn::Conv2dOptions(numFilters, numFilters, 3).padding(1)));
		normBody11 = register_module("normBody11", torch::nn::BatchNorm2d(numFilters));

		convValue = register_module("convValue", torch::nn::Conv2d(torch::nn::Conv2dOptions(numFilters, valueFilters, 1)));
		normValue = register_module("normValue", torch::nn::BatchNorm2d(valueFilters));

		linearValue1 = register_module("linearValue1", torch::nn::Linear(valueLinearSize, 256));
		linearValue2 = register_module("linearValue2", torch::nn::Linear(256, 1));

		convPolicy = register_module("convPolicy", torch::nn::Conv2d(torch::nn::Conv2dOptions(numFilters, policyFilters, 1)));
		normPolicy = register_module("normPolicy", torch::nn::BatchNorm2d(policyFilters));
		linearPolicy = register_module("linearPolicy", torch::nn::Linear(policyLinearSize, numOutputs));
	}


	std::tuple<torch::Tensor, torch::Tensor> calculate(const torch::Tensor& input) {
		return forward(input);
	}


	std::tuple<torch::Tensor, torch::Tensor> forward(const torch::Tensor& input) {
		int64_t batchSize = input.size(0);

		//Convolution Block 0
		auto x = torch::relu(normBody1(convBody1(input)));

		//Residual Block 1
		auto buf = x;
		x = torch::relu(normBody2(convBody2(x)));
		x = normBody3(convBody3(x));
		x = torch::relu(x + buf);

		//Residual Block 2
		buf = x;
		x = torch::relu(normBody4(convBody4(x)));
		x = normBody5(convBody5(x));
		x = torch::relu(x + buf);

		//Residual Block 3
		buf = x;
		x = torch::relu(normBody6(convBody6(x)));
		x = normBody7(convBody7(x));
		x = torch::relu(x + buf);

		//Residual Block 4
		buf = x;
		x = torch::relu(normBody8(convBody8(x)));
		x = normBody9(convBody9(x));
		x = torch::relu(x + buf);

		//Residual Block 5
		buf = x;
		x = torch::relu(normBody10(convBody10(x)));
		x = normBody11(convBody11(x));
		x = torch::relu(x + buf);


		auto pol = torch::relu(normPolicy(convPolicy(x)));
		pol = pol.view({ batchSize, -1 });
		pol = linearPolicy(pol);
		pol = torch::softmax(pol, 1);


		auto val = torch::relu(normValue(convValue(x)));
		val = val.view({ batchSize, -1 });
		val = torch::relu(linearValue1(val));
		val = torch::tanh(linearValue2(val));

		return std::make_tuple(val, pol);
	}

	int64_t numFilters = 256;


	torch::nn::Conv2d convBody1 = nullptr;
	torch::nn::Conv2d convBody2 = nullptr;
	torch::nn::Conv2d convBody3 = nullptr;
	torch::nn::Conv2d convBody4 = nullptr;
	torch::nn::Conv2d convBody5 = nullptr;
	torch::nn::Conv2d convBody6 = nullptr;
	torch::nn::Conv2d convBody7 = nullptr;
	torch::nn::Conv2d convBody8 = nullptr;
	torch::nn::Conv2d convBody9 = nullptr;
	torch::nn::Conv2d convBody10 = nullptr;
	torch::nn::Conv2d convBody11 = nullptr;

	torch::nn::BatchNorm2d normBody1 = nullptr;
	torch::nn::BatchNorm2d normBody2 = nullptr;
	torch::nn::BatchNorm2d normBody3 = nullptr;
	torch::nn::BatchNorm2d normBody4 = nullptr;
	torch::nn::BatchNorm2d normBody5 = nullptr;
	torch::nn::BatchNorm2d normBody6 = nullptr;
	torch::nn::BatchNorm2d normBody7 = nullptr;
	torch::nn::BatchNorm2d normBody8 = nullptr;
	torch::nn::BatchNorm2d normBody9 = nullptr;
	torch::nn::BatchNorm2d normBody10 = nullptr;
	torch::nn::BatchNorm2d normBody11 = nullptr;

	torch::nn::Conv2d convValue = nullptr;
	torch::nn::BatchNorm2d normValue = nullptr;
	torch::nn::Linear linearValue1 = nullptr;
	torch::nn::Linear linearValue2 = nullptr;

	torch::nn::Conv2d convPolicy = nullptr;
	torch::nn::BatchNorm2d normPolicy = nullptr;
	torch::nn::Linear linearPolicy = nullptr;

	int width;
	int height;
};
TORCH_MODULE_IMPL(NoResidualNN, NoResidualNNImpl);


#endif //DEEPREINFORCEMENTLEARNING_NORESIDUALNNIMPL_H

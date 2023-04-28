#include <gtest/gtest.h>
#include <MonteCarloTreeSearch.h>
#include <NeuralNetworks/DefaultNeuralNet.h>
#include <ConnectFour/ConnectFourAdapter.h>
#include <vector>
#include <MonteCarloTreeSearchTemplate.h>

#include "TestConfig.h"
#include "Test_Utility.h"


#if RunTests

static torch::DeviceType device = torch::kCPU;
static ConnectFourAdapter cn4Adap = ConnectFourAdapter();
static DefaultNeuralNet net(2, 3, 3, 9);

//TEST(MCTS_ConnectFour, test_mcts_cn4_yellow_wins)
//{
//	const std::string boardStr = "002120000012000001000000100000000000000000";
//	auto board = cn4::Board(boardStr);
//	auto mctsCache = MonteCarloTreeSearchCacheT<cn4::Board, ConnectFourAdapter>(torch::kCPU, &cn4Adap);
//	//auto mcts = MonteCarloTreeSearchT<cn4::Board, ConnectFourAdapter>(&mctsCache, &cn4Adap, device);
//	//float result = mcts.search(board, &net, 1);
//
//	ASSERT_FLOAT_EQ(1, 0);
//}

#endif //RunTests



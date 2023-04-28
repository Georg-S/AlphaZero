#include <gtest/gtest.h>
#include <MonteCarloTreeSearchTemplate.h>
#include <MonteCarloTreeSearch.h>
#include <NeuralNetworks/DefaultNeuralNet.h>
#include <Chess/ChessAdapter.h>
#include <tuple>
#include <thread>
#include <vector>

#include "TestConfig.h"
#include "Test_Utility.h"

#if RunTests

static torch::DeviceType device = torch::kCUDA;
static ChessAdapter chessAdap = ChessAdapter();
static DefaultNeuralNet neuralNet = DefaultNeuralNet(14, 8, 8, 4096, device);

#if RunLongTests

TEST(MonteCarloTreeSearch, test_chess_one_move_wins)
{
	torch::DeviceType device = torch::kCUDA;
	const std::string gameState = "8/8/8/p7/Pn6/KPp5/RR6/r6k w - - 0 1";
	const int winningMove = chess::getIntFromMove({ 0,6,0,7 });
	
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(chessAdap.getActionCount(), device, &chessAdap);

	mcts.search(100, gameState, &neuralNet, static_cast<int>(ceg::PieceColor::WHITE));
	auto probabilities = mcts.getProbabilities(gameState);
	int mctsBestMove = ALZ::getBestAction(probabilities);

	ASSERT_EQ(winningMove, mctsBestMove);
}
#endif //RunLongTests

#endif //RunTests
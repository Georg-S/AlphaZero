#ifndef DEEPREINFORCEMENTLEARNING_cn4_MINIMAXAI_H
#define DEEPREINFORCEMENTLEARNING_cn4_MINIMAXAI_H

#include <algorithm>
#include "Other/Ai.h"
#include "GameLogic.h"

namespace cn4
{
	class MiniMaxAi : public Ai
	{
	public:
		MiniMaxAi(int depth = 5);
		int getMove(const std::string& state, int playerColor) override;

	private:
		int evaluateBoard(const cn4::Board& board, int depth, PlayerColor currentPlayer, bool maximizingPlayer, int alpha, int beta);
		int staticBoardEvaluation(const cn4::Board& board);
		void orderMoves(std::vector<int>& moves) const;
		std::vector<int> getBestMoves(std::vector<int>& moves, std::vector<int>& values);

		PlayerColor m_aiColor;
		int m_depth;
		static constexpr int m_maxValue = 10000;
		static constexpr int m_minValue = -10000;
	};
}

#endif //DEEPREINFORCEMENTLEARNING_cn4_MINIMAXAI_H

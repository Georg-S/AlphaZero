#ifndef DEEPREINFORCEMENTLEARNING_cn4_MINIMAXAI_H
#define DEEPREINFORCEMENTLEARNING_cn4_MINIMAXAI_H

#include <algorithm>
#include "Other/Ai.h"
#include "GameLogic.h"

namespace cn4
{
	class NegaMaxAi : public Ai
	{
	public:
		NegaMaxAi(int depth = 5);
		int getMove(const std::string& state, int playerColor) override;

	private:
		int negamax(const cn4::Board& board, int depth, PlayerColor currentPlayer, int alpha, int beta) const;
		void orderMoves(std::vector<int>& moves) const;
		int staticBoardEvaluation(const cn4::Board& board, PlayerColor currentPlayer) const;

		int m_depth = 5;
		static constexpr int m_maxValue = 10000;
		static constexpr int m_minValue = -10000;
		static constexpr int m_loseValue = -3000;
	};
}

#endif //DEEPREINFORCEMENTLEARNING_cn4_MINIMAXAI_H

#ifndef DEEPREINFORCEMENTLEARNING_ttt_MINIMAXAI_H
#define DEEPREINFORCEMENTLEARNING_ttt_MINIMAXAI_H

#include <vector>
#include <climits>
#include <algorithm>
#include <AlphaZeroUtility.h>
#include "Other/Ai.h"
#include "Other/Utility.h"
#include "TicTacToe/GameLogic.h"

namespace ttt
{
	class MiniMaxAi : public Ai
	{
	public:
		MiniMaxAi() = default;
		int getMove(const std::string& state, int color) override;

	private:
		ttt::Move getMove(ttt::Board board, int color);
		int evaluateBoard(const ttt::Board& board, PlayerColor currentPlayer, bool maximizingPlayer, int alpha, int beta);
		std::vector<ttt::Move> getBestMoves(const std::vector<ttt::Move>& moves, const std::vector<int>& values);

		PlayerColor m_color;
		PlayerColor m_opponentColor;
	};
}


#endif //DEEPREINFORCEMENTLEARNING_ttt_MINIMAXAI_H

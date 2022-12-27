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

		PlayerColor m_color = PlayerColor::NONE;
		PlayerColor m_opponentColor = PlayerColor::NONE;
	};
}


#endif //DEEPREINFORCEMENTLEARNING_ttt_MINIMAXAI_H

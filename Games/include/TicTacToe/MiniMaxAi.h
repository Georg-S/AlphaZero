#ifndef DEEPREINFORCEMENTLEARNING_ttt_MINIMAXAI_H
#define DEEPREINFORCEMENTLEARNING_ttt_MINIMAXAI_H

#include <vector>
#include <climits>
#include <algorithm>
#include "AI/Ai.h"
#include "TicTacToe/GameLogic.h"
#include "TicTacToe/TicTacToeAdapter.h"
#include "TicTacToe/Move.h"
#include "TicTacToe/Board.h"

namespace ttt {
	class MiniMaxAi : public Ai {
	public:
		MiniMaxAi();
		int getMove(std::string state, int color) override;

	private:
		ttt::Move getMove(ttt::Board board, int color);
		int evaluateBoard(ttt::Board board, int currentPlayer, bool maximizingPlayer, int alpha, int beta);
		std::vector<ttt::Move> getAllPossibleMoves(const ttt::Board& board);
		std::vector<ttt::Move> getBestMoves(std::vector<ttt::Move>& moves, std::vector<int>& values);
		ttt::Move getRandomMove(const std::vector<ttt::Move>& moves);

		int color;
		int opponentColor;
	};
}


#endif //DEEPREINFORCEMENTLEARNING_ttt_MINIMAXAI_H

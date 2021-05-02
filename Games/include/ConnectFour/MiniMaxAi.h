#ifndef DEEPREINFORCEMENTLEARNING_cn4_MINIMAXAI_H
#define DEEPREINFORCEMENTLEARNING_cn4_MINIMAXAI_H

#include "AI/Ai.h"
#include "GameLogic.h"
#include "Board.h"
#include "ConnectFourAdapter.h"

namespace cn4
{
	class MiniMaxAi : public Ai
	{
	public:
		MiniMaxAi(int depth = 5);
		int getMove(std::string state, int playerColor) override;

	private:
		int evaluateBoard(const cn4::Board& board, int depth, int currentPlayer, bool maximizingPlayer, int alpha, int beta);
		int staticBoardEvaluation(const cn4::Board& board);
		int evaluateBoardPosition(const cn4::Board& board, int x, int y);
		int rightHorizontalValue(const cn4::Board& board, int x, int y);
		int rightDiagonalDownValue(const cn4::Board& board, int x, int y);
		int rightDiagonalUpValue(const cn4::Board& board, int x, int y);
		int upVerticalValue(const cn4::Board& board, int x, int y);
		void makeMove(cn4::Board& board, int move, int currenPlayer);
		void getAllPossibleMoves(std::vector<int>& destination, const cn4::Board& board) const;
		void orderMoves(std::vector<int>& moves) const;
		std::vector<int> getBestMoves(std::vector<int>& moves, std::vector<int>& values);
		int getRandomMove(const std::vector<int>& moves);

		int aiColor;
		int depth;
		ConnectFourAdapter adap;
		const int maxValue = 10000;
		const int minValue = -10000;
	};
}

#endif //DEEPREINFORCEMENTLEARNING_cn4_MINIMAXAI_H

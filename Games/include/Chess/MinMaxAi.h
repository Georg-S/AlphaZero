#ifndef DEEPREINFORCEMENTLEARNING_chess_MINMAXAI_H
#define DEEPREINFORCEMENTLEARNING_chess_MINMAXAI_H

#include <vector>
#include "Chess/ReducedChessAdapter.h"
#include "AI/Ai.h"
#include "Pieces/Queen.h"

namespace chess {
	class MinMaxAi : public Ai {
	public:
		MinMaxAi(int depth);
		int getMove(std::string state, int playerColor) override;

	private:
		int evaluateBoard(chess::Board& board, int depth, chess::PieceColor currentPlayer, bool maximizingPlayer, int alpha, int beta);
		int staticEvaluateBoard(chess::Board& board);
		void makeMoveWithPromotion(chess::Board& board, const Move& move);
		void replacePawnsInRowWithQueens(chess::Board& board, int row);
		std::vector<chess::Move> getAllPossibleMoves(chess::Board& board, const chess::PieceColor& color);
		void getAllPossibleMovesForPiece(chess::Board& board, int fromX, int fromY, std::vector<chess::Move>& moves);
		int getPieceValue(Piece* piece);

		std::vector<chess::Move> getBestMoves(std::vector<chess::Move>& moves, std::vector<int>& values);
		chess::Move getRandomMove(const std::vector<chess::Move>& moves);

		chess::PieceColor aiColor;
		chess::PieceColor otherPlayer;
		int depth;
		const int checkMateValue = 10000;
		const int minValue = -12000;
		const int maxValue = 12000;
	};
}

#endif //DEEPREINFORCEMENTLEARNING_chess_MINMAXAI_H

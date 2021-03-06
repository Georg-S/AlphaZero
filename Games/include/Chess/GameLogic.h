#ifndef DEEPREINFORCEMENTLEARNING_chess_GAMELOGIC_H
#define DEEPREINFORCEMENTLEARNING_chess_GAMELOGIC_H

#include "Board.h"
#include "Move.h"

namespace chess
{
	class GameLogic
	{
	public:
		static bool isFieldEmpty(const chess::Board& board, int x, int y);
		static void makeMove(chess::Board& board, const chess::Move& move);
		static bool isGameOver(chess::Board& board, chess::PieceColor currentPlayer);
		static bool isMoveValid(chess::Board& board, const chess::Move& move);
		static bool isCheck(chess::Board& board, const chess::PieceColor& pieceColor);
		static bool isCheckMate(chess::Board& board, const chess::PieceColor& pieceColor);
		static bool isStaleMate(chess::Board& board, const chess::PieceColor& pieceColor);
		static bool pawnReachedEndOfBoard(const chess::Board& board);
		static chess::PieceColor getNextPlayer(const chess::PieceColor color);
		static bool isAnyMovePossible(chess::Board& board, const chess::PieceColor& color);

	private:
		static bool isAnyMoveWithPiecePossible(chess::Board& board, int pieceX, int pieceY);
		static bool isPawnInRow(const chess::Board& board, int row);
	};
}

#endif //DEEPREINFORCEMENTLEARNING_chess_GAMELOGIC_H

#ifndef DEEPREINFORCEMENTLEARNING_chess_BOARD_H
#define DEEPREINFORCEMENTLEARNING_chess_BOARD_H

#include <string>
#include "PieceColor.h"
#include "Pieces/Piece.h"
#include "Pieces/Bishop.h"
#include "Pieces/King.h"
#include "Pieces/Knight.h"
#include "Pieces/Pawn.h"
#include "Pieces/Queen.h"
#include "Pieces/Rook.h"

namespace chess
{
	struct Board
	{
	public:
		Piece* board[8][8];

		Board()
		{
			zeroInitialize();
		}

		Board(const Board& board)
		{
			zeroInitialize();

			for (int x = 0; x < 8; x++)
			{
				for (int y = 0; y < 8; y++)
				{
					if (board.board[x][y] != nullptr)
						this->board[x][y] = board.board[x][y]->getDeepCopy();
				}
			}
		}

		void setToInitialState()
		{
			zeroInitialize();
			initialStateBlackSite();
			initialStateWhiteSite();
		}

		void deletePieces()
		{
			for (int x = 0; x < 8; x++)
			{
				for (int y = 0; y < 8; y++)
				{
					if (board[x][y] != nullptr)
					{
						delete board[x][y];
						board[x][y] = nullptr;
					}
				}
			}
		}

	private:
		void initialStateBlackSite()
		{
			board[0][0] = new Rook(chess::PieceColor::BLACK);
			board[7][0] = new Rook(chess::PieceColor::BLACK);

			board[1][0] = new Knight(chess::PieceColor::BLACK);
			board[6][0] = new Knight(chess::PieceColor::BLACK);

			board[2][0] = new Bishop(chess::PieceColor::BLACK);
			board[5][0] = new Bishop(chess::PieceColor::BLACK);

			board[3][0] = new Queen(chess::PieceColor::BLACK);
			board[4][0] = new King(chess::PieceColor::BLACK);

			for (int i = 0; i < 8; i++)
			{
				board[i][1] = new Pawn(chess::PieceColor::BLACK);
			}
		}

		void initialStateWhiteSite()
		{
			board[0][7] = new Rook(chess::PieceColor::WHITE);
			board[7][7] = new Rook(chess::PieceColor::WHITE);

			board[1][7] = new Knight(chess::PieceColor::WHITE);
			board[6][7] = new Knight(chess::PieceColor::WHITE);

			board[2][7] = new Bishop(chess::PieceColor::WHITE);
			board[5][7] = new Bishop(chess::PieceColor::WHITE);

			board[3][7] = new Queen(chess::PieceColor::WHITE);
			board[4][7] = new King(chess::PieceColor::WHITE);

			for (int i = 0; i < 8; i++)
			{
				board[i][6] = new Pawn(chess::PieceColor::WHITE);
			}
		}

		void zeroInitialize()
		{
			for (int x = 0; x < 8; x++)
			{
				for (int y = 0; y < 8; y++)
				{
					board[x][y] = nullptr;
				}
			}
		}
	};
}

#endif //DEEPREINFORCEMENTLEARNING_chess_BOARD_H
#ifndef DEEPREINFORCEMENTLEARNING_chess_RENDERER_H
#define DEEPREINFORCEMENTLEARNING_chess_RENDERER_H

#include "SDL/SDLHandler.h"
#include "Board.h"
#include "RenderInformation.h"

namespace chess
{
	class Renderer
	{
	public:
		Renderer();
		~Renderer();
		void render(const chess::RenderInformation& renderInfo);
		void renderPromotionSelection(chess::PieceColor color);
		int getWindowWidth();
		int getWindowHeight();
		void updateQuit();
		bool isQuit();
		void quit();

	private:
		void renderChessBoard();
		void renderPieces(const chess::Board& board);
		void renderPiecesWithSelectedOnMousePosition(const chess::RenderInformation& renderInfo);
		void renderPreviousMove(const Move& previousMove);
		void renderPiece(const chess::Board& board, int x, int y);
		void renderPieceOnMousePosition(Piece* piece, int mouseX, int mouseY);
		std::string getFileString(Piece* piece);
		std::string getFileString(const char& pieceChar, const chess::PieceColor& pieceColor);

		const int windowWidth = 800;
		const int windowHeight = 800;
		int pieceWidth;
		int pieceHeight;
		SDLHandler* sdlHandler;
	};
}

#endif //DEEPREINFORCEMENTLEARNING_chess_RENDERER_H

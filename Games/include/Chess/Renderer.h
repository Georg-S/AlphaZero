#ifndef DEEPREINFORCEMENTLEARNING_chess_RENDERER_H
#define DEEPREINFORCEMENTLEARNING_chess_RENDERER_H

#include "SDL/SDLHandler.h"
#include "Board.h"
#include "RenderInformation.h"

namespace chess {
    class Renderer {
    public:
        Renderer();
        ~Renderer();

        void render(const chess::RenderInformation &renderInfo);
        void renderBoard(const chess::Board &board);
        void renderPromotionSelection(chess::PieceColor color);

        int getWindowWidth();
        int getWindowHeight();

        void updateQuit();
        bool isQuit();
        void quit();
    private:
        void renderBoardWithPieceOnMousePosition(const chess::RenderInformation &renderInfo);
        void renderPiece(const chess::Board &board, const int &x, const int &y);
        void renderPieceOnMousePosition(Piece* piece, const int &mouseX, const int &mouseY);
        std::string getFileString(Piece* piece);
        std::string getFileString(const char &pieceChar, const chess::PieceColor &pieceColor);

        const int windowWidth = 800;
        const int windowHeight = 800;
        int pieceWidth;
        int pieceHeight;

        SDLHandler* sdlHandler;
    };
}

#endif //DEEPREINFORCEMENTLEARNING_chess_RENDERER_H

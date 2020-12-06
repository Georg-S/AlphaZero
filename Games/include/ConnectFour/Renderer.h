#ifndef DEEPREINFORCEMENTLEARNING_cn4_RENDERER_H
#define DEEPREINFORCEMENTLEARNING_cn4_RENDERER_H


#include "SDL/SDLHandler.h"
#include "Board.h"

namespace cn4 {
    class Renderer {
    public:
        Renderer();
        ~Renderer();

        void renderBoard(cn4::Board board);
        int getWindowXPosition();
        int getWindowYPosition();
        void quit();
        bool isQuit();
        void updateQuit();

        const int windowWidth = 700;
        const int windowHeight = 600;
    private:
        void renderPiece(const cn4::Board &board, const int &x, const int &y);
        int convertBoardXPositionToRenderXPosition(const cn4::Board &board, const int &x);
        int convertBoardYPositionToRenderYPosition(const cn4::Board &board, const int &y);

        SDLHandler *handler;
    };
}

#endif //DEEPREINFORCEMENTLEARNING_cn4_RENDERER_H

#ifndef DEEPREINFORCEMENTLEARNING_ttt_RENDERER_H
#define DEEPREINFORCEMENTLEARNING_ttt_RENDERER_H

#include "Board.h"
#include "SDL/SDLHandler.h"

namespace ttt {
    class Renderer {
    public:
        Renderer();
        ~Renderer();

        void renderBoard(const ttt::Board &board);
        void updateQuit();
        void quit();
        bool isQuit();

        static const int windowWidth = 800;
        static const int windowHeight = 800;
    private:
        SDLHandler* sdlHandler;

        int pieceWidth;
        int pieceHeight;
    };
}

#endif //DEEPREINFORCEMENTLEARNING_ttt_RENDERER_H

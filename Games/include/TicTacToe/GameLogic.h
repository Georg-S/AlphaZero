#ifndef DEEPREINFORCEMENTLEARNING_ttt_GAMELOGIC_H
#define DEEPREINFORCEMENTLEARNING_ttt_GAMELOGIC_H

#include "Board.h"
#include "Move.h"

namespace ttt {
    class GameLogic {
    public:
        static bool isGameOver(const ttt::Board &board);
        static bool isBoardFull(const ttt::Board &board);
        static int getPlayerWon(const ttt::Board &board);
        static bool playerWon(const ttt::Board &board, const int &player);
        static int getNextPlayer(const int &currentPlayer);
        static bool isMovePossible(const ttt::Board &board, const ttt::Move &move);
        static void makeMove(ttt::Board &board, const int &currentPlayer, const ttt::Move &move);

    private:
        static bool isThreeInARowHorizontal(const ttt::Board &board, const int &player = -1);
        static bool isThreeInARowHorizontal(const ttt::Board &board, const int &y, const int &player);
        static bool isThreeInARowVertical(const ttt::Board &board, const int &player = -1);
        static bool isThreeInARowVertical(const ttt::Board &board, const int &x, const int &player);
        static bool isThreeInARowDiagonal(const ttt::Board &board, const int &player = -1);
        static bool isThreeInARowDiagonalLeftCorner(const ttt::Board &board, const int &player);
        static bool isThreeInARowDiagonalRightCorner(const ttt::Board &board, const int &player);
    };
}
#endif //DEEPREINFORCEMENTLEARNING_ttt_GAMELOGIC_H

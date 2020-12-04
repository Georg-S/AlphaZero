#ifndef PROTOTYPE_cn4_GAMELOGIC_H
#define PROTOTYPE_cn4_GAMELOGIC_H

#include "PlayerColor.h"
#include "Board.h"

namespace cn4 {
    class GameLogic {
    public:
        static cn4::PlayerColor getPlayerWon(const cn4::Board &board);
        static bool isGameOver(const cn4::Board &board);
        static bool isDraw(const cn4::Board &board);
        static void makeMove(cn4::Board &board, const int &action, const cn4::PlayerColor &playerColor);
        static bool isMovePossible(const cn4::Board &board, const int &action);


    private:

        static bool fourInARowHorizontal(const cn4::Board &board, const int &player = -1);
        static bool fourInARowHorizontal(const cn4::Board &board, const int &player, const int &xPos, const int &yPos);
        static bool fourInARowVertical(const cn4::Board &board, const int &player = -1);
        static bool fourInARowVertical(const cn4::Board &board, const int &player, const int &xPos, const int &yPos);
        static bool fourInARowDiagonal(const cn4::Board &board, const int &player = -1);
        static bool fourInARowDiagonalDown(const cn4::Board &board, const int &player, const int &xPos, const int &yPos);
        static bool fourInARowDiagonalUp(const cn4::Board &board, const int &player, const int &xPos, const int &yPos);
    };
}

#endif //PROTOTYPE_cn4_GAMELOGIC_H

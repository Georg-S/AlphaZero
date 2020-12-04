
#ifndef PROTOTYPE_cn4_BOARD_H
#define PROTOTYPE_cn4_BOARD_H

#include "PlayerColor.h"

namespace cn4 {
    struct Board {
        Board() {
            for (int x = 0; x < width; x++)
                for (int y = 0; y < height; y++)
                    board[x][y] = cn4::PlayerColor::NONE;
        }

        Board(const cn4::Board &board) {
            for (int x = 0; x < width; x++) {
                for (int y = 0; y < height; y++) {
                    this->board[x][y] = board.board[x][y];
                }
            }
        }

        const static int width = 7;
        const static int height = 6;
        int board[width][height];
    };
}

#endif //PROTOTYPE_cn4_BOARD_H

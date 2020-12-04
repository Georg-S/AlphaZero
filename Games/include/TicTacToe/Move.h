#ifndef PROTOTYPE_ttt_MOVE_H
#define PROTOTYPE_ttt_MOVE_H

namespace ttt {
    struct Move {
        Move(int x, int y) {
            this->x = x;
            this->y = y;
        }

        int x, y;
    };
}

#endif //PROTOTYPE_ttt_MOVE_H

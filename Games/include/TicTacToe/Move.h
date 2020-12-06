#ifndef DEEPREINFORCEMENTLEARNING_ttt_MOVE_H
#define DEEPREINFORCEMENTLEARNING_ttt_MOVE_H

namespace ttt {
    struct Move {
        Move(int x, int y) {
            this->x = x;
            this->y = y;
        }

        int x, y;
    };
}

#endif //DEEPREINFORCEMENTLEARNING_ttt_MOVE_H

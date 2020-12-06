#ifndef DEEPREINFORCEMENTLEARNING_chess_MOVE_H
#define DEEPREINFORCEMENTLEARNING_chess_MOVE_H

namespace chess {
	struct Move {
		Move() {
			this->fromX = 0;
			this->fromY = 0;
			this->toX = 0;
			this->toY = 0;
		}

		Move(int fromX, int fromY, int toX, int toY) {
			this->fromX = fromX;
			this->fromY = fromY;
			this->toX = toX;
			this->toY = toY;
		}

		Move(int intRepresentation) {
			int from = intRepresentation % 64;
			int to = intRepresentation / 64;

			this->fromX = from % 8;
			this->fromY = from / 8;
			this->toX = to % 8;
			this->toY = to / 8;
		}

		int getSingleIntRepresentation() {
			int from = this->fromX + this->fromY * 8;
			int to = this->toX + this->toY * 8;

			return from + to * 64;
		}

		int fromX;
		int fromY;
		int toX;
		int toY;
	};
}

#endif //DEEPREINFORCEMENTLEARNING_chess_MOVE_H

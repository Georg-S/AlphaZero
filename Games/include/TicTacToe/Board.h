#ifndef DEEPREINFORCEMENTLEARNING_ttt_BOARD_H
#define DEEPREINFORCEMENTLEARNING_ttt_BOARD_H

namespace ttt {
	struct Board {
		Board() {
			for (int x = 0; x < 3; x++)
				for (int y = 0; y < 3; y++)
					board[x][y] = 0;
		}

		Board(const ttt::Board& board) {
			for (int x = 0; x < 3; x++)
				for (int y = 0; y < 3; y++)
					this->board[x][y] = board.board[x][y];
		}


		int board[3][3];
	};
}

#endif //DEEPREINFORCEMENTLEARNING_ttt_BOARD_H

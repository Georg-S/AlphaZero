#ifndef DEEPREINFORCEMENTLEARNING_cn4_GAMELOGIC_H
#define DEEPREINFORCEMENTLEARNING_cn4_GAMELOGIC_H

#include "PlayerColor.h"
#include "Board.h"

namespace cn4 {
	class GameLogic {
	public:
		static cn4::PlayerColor getPlayerWon(const cn4::Board& board);
		static bool isGameOver(const cn4::Board& board);
		static bool isDraw(const cn4::Board& board);
		static void makeMove(cn4::Board& board, int action, const cn4::PlayerColor& playerColor);
		static bool isMovePossible(const cn4::Board& board, int action);


	private:

		static bool fourInARowHorizontal(const cn4::Board& board, int player = -1);
		static bool fourInARowHorizontal(const cn4::Board& board, int player, int xPos, int yPos);
		static bool fourInARowVertical(const cn4::Board& board, int player = -1);
		static bool fourInARowVertical(const cn4::Board& board, int player, int xPos, int yPos);
		static bool fourInARowDiagonal(const cn4::Board& board, int player = -1);
		static bool fourInARowDiagonalDown(const cn4::Board& board, int player, int xPos, int yPos);
		static bool fourInARowDiagonalUp(const cn4::Board& board, int player, int xPos, int yPos);
	};
}

#endif //DEEPREINFORCEMENTLEARNING_cn4_GAMELOGIC_H

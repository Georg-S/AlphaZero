#ifndef DEEPREINFORCEMENTLEARNING_chess_RENDERINFORMATION_H
#define DEEPREINFORCEMENTLEARNING_chess_RENDERINFORMATION_H

#include "Board.h"

namespace chess {
	struct RenderInformation {
		RenderInformation() {
			board = Board();
			selectedPieceX = -1;
			selectedPieceY = -1;
			mousePositionX = 0;
			mousePositionY = 0;
		}

		RenderInformation(chess::Board& board, int selectedX, int selectedY, int mouseX, int mouseY) {
			this->board = board;
			this->selectedPieceX = selectedX;
			this->selectedPieceY = selectedY;
			this->mousePositionX = mouseX;
			this->mousePositionY = mouseY;
		}

		chess::Board board;
		int selectedPieceX;
		int selectedPieceY;
		int mousePositionX;
		int mousePositionY;
	};
}

#endif //DEEPREINFORCEMENTLEARNING_chess_RENDERINFORMATION_H

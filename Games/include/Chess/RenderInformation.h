#ifndef DEEPREINFORCEMENTLEARNING_chess_RENDERINFORMATION_H
#define DEEPREINFORCEMENTLEARNING_chess_RENDERINFORMATION_H

#include "Board.h"

namespace chess
{
	struct RenderInformation
	{
		RenderInformation()
		{

		}

		RenderInformation(chess::Board& board, int selectedX = -1, int selectedY = -1, int mouseX = 0, int mouseY = 0,
			chess::Move previousMove = Move(-1, -1, -1, -1))
		{
			this->board = board;
			this->selectedPieceX = selectedX;
			this->selectedPieceY = selectedY;
			this->mousePositionX = mouseX;
			this->mousePositionY = mouseY;
			this->previousMove = previousMove;
		}

		RenderInformation(chess::Board& board, chess::Move previousMove)
		{
			this->board = board;
			this->previousMove = previousMove;

			this->selectedPieceX = -1;
			this->selectedPieceY = -1;
			this->mousePositionX = 0;
			this->mousePositionY = 0;
		}

		chess::Board board;
		int selectedPieceX;
		int selectedPieceY;
		int mousePositionX;
		int mousePositionY;
		chess::Move previousMove;
	};
}

#endif //DEEPREINFORCEMENTLEARNING_chess_RENDERINFORMATION_H

#pragma once
#include "Engine/ChessEngine.h"
#include "Engine/BitBoard.h"

struct RenderInformation
{
	RenderInformation(const ceg::BitBoard& board, int selectedX = -1, int selectedY = -1, int mouseX = 0, int mouseY = 0,
		ceg::Move previousMove = ceg::Move{ -1, -1, -1, -1 })
	{
		this->board = board;
		this->selectedPieceX = selectedX;
		this->selectedPieceY = selectedY;
		this->mousePositionX = mouseX;
		this->mousePositionY = mouseY;
		this->previousMove = previousMove;
		this->check_mate = false;
		this->stale_mate = false;
	}
	RenderInformation(const ceg::BitBoard& board, ceg::Move previousMove) : RenderInformation(board, -1, -1, 0, 0, previousMove) {}

	ceg::BitBoard board;
	int selectedPieceX;
	int selectedPieceY;
	int mousePositionX;
	int mousePositionY;
	ceg::Move previousMove;
	bool check_mate;
	bool stale_mate;
};

#ifndef DEEPREINFORCEMENTLEARNING_CHESS_H
#define DEEPREINFORCEMENTLEARNING_CHESS_H

#include <iostream>
#include <chrono>
#include <thread>
#include "AI/Ai.h"
#include "SDL/Mouse.h"
#include "Renderer.h"
#include "RenderInformation.h"
#include "Pieces/Piece.h"
#include "GameLogic.h"
#include "PieceColor.h"
#include "Move.h"
#include "Board.h"
#include "ReducedChessAdapter.h"


class Chess
{
public:
	Chess();
	Chess(chess::PieceColor playerColor, Ai* ai);
	void gameLoop();
	void updateGame();

private:
	void update1PlayerGame();
	void update2PlayerGame();
	void updateAiMove();
	void updateHumanMove();
	void outputBoard();
	chess::Move getMove();
	chess::Move getMoveFromKeyboardInput();
	chess::Move getMoveFromGraphicalInput();
	chess::Piece* getPieceFromPromoSelection(int mouseX, int mouseY);
	static void printBoardToConsole(const chess::Board& board);
	bool isValidMove(const chess::Move& move);
	void handleGameOver();
	int convertMousePositionXToBoardPosition(int mouseX);
	int convertMousePositionYToBoardPosition(int mouseY);
	bool isOutOfRange(int boardPosition);
	void handlePromoSelection(chess::Board& board, int pawnX, int pawnY);

	Mouse mouse;
	Ai* ai;
	chess::Renderer* renderer;
	chess::Board board;
	bool gameOver = false;
	bool graphicalOutput = true;
	int playerCount = 2;
	chess::PieceColor currentPlayer = chess::PieceColor::WHITE;
	chess::PieceColor aiColor;
	int windowWidth;
	int windowHeight;
	int selectedPieceX = -1;
	int selectedPieceY = -1;
	bool pieceSelected = false;
	chess::Move previousMove = chess::Move(-1, -1, -1, -1);;
};


#endif //DEEPREINFORCEMENTLEARNING_CHESS_H

#ifndef DEEPREINFORCEMENTLEARNING_CONNECTFOUR_H
#define DEEPREINFORCEMENTLEARNING_CONNECTFOUR_H

#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include "GameLogic.h"
#include "SDL/Mouse.h"
#include "Renderer.h"
#include "Board.h"
#include "AI/Ai.h"
#include "PlayerColor.h"


class ConnectFour {

public:
	ConnectFour();
	ConnectFour(cn4::PlayerColor aiColor, Ai* ai);
	void gameLoop();
	void updateGame();

private:
	void update1PlayerGame();
	void update2PlayerGame();
	void updateAiMove();
	void updateHumanMove();
	int getAction();
	int getActionFromMouseInput();
	int getActionFromKeyboard();
	static bool isValidInput(const cn4::Board& board, int action);
	static char convertPlayerColorToChar(const cn4::PlayerColor& playerColor);
	void handleGameOver(const cn4::Board& board);
	static void printBoardToConsole(const cn4::Board& board);
	static cn4::PlayerColor getNextPlayer(const cn4::PlayerColor& player);
	void outputBoard(const cn4::Board& board);

	Ai* ai;
	cn4::PlayerColor currentPlayerColor;
	cn4::PlayerColor aiColor;
	int playerCount;
	cn4::Board board;
	bool gameOver = false;
	bool useGraphicalRender = true;
	cn4::Renderer* renderer;
	Mouse mouse = Mouse();
};


#endif //DEEPREINFORCEMENTLEARNING_CONNECTFOUR_H

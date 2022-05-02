#ifndef DEEPREINFORCEMENTLEARNING_CONNECTFOUR_H
#define DEEPREINFORCEMENTLEARNING_CONNECTFOUR_H

#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include "GameLogic.h"
#include "SDL/Mouse.h"
#include "Renderer.h"
#include "Other/Ai.h"

class ConnectFour
{
public:
	ConnectFour();
	ConnectFour(cn4::PlayerColor aiColor, Ai* ai);
	void gameLoop();
	void updateGame();

private:
	static bool isValidInput(const cn4::Board& board, int action);
	void update1PlayerGame();
	void update2PlayerGame();
	void updateAiMove();
	void updateHumanMove();
	int getActionFromMouseInput();

	Ai* m_ai;
	cn4::PlayerColor m_currentPlayerColor;
	cn4::PlayerColor m_aiColor;
	int m_playerCount;
	cn4::Board m_board;
	bool m_gameOver = false;
	std::unique_ptr<cn4::Renderer> m_renderer;
	Mouse m_mouse = Mouse();
};


#endif //DEEPREINFORCEMENTLEARNING_CONNECTFOUR_H

#ifndef DEEPREINFORCEMENTLEARNING_TICTACTOE_H
#define DEEPREINFORCEMENTLEARNING_TICTACTOE_H

#include <iostream>
#include <chrono>
#include <thread>
#include <memory>
#include "GameLogic.h"
#include "Other/Ai.h"
#include "Renderer.h"
#include "SDL/Mouse.h"

class TicTacToe
{
public:
	TicTacToe();
	TicTacToe(Ai* ai, ttt::PlayerColor aiColor);
	void updateGame();
	void gameLoop();

private:
	void updateTwoPlayerGame();
	void updateOnePlayerGame();
	void updateAiMove(ttt::PlayerColor currentPlayer, ttt::Board& board);
	void updateHumanPlayerMove(ttt::PlayerColor currentPlayer, ttt::Board& board);
	void handleGameOver();
	bool isValid(const ttt::Move& move);
	bool isValidMove(const ttt::Move& move, const ttt::Board& board);
	ttt::Move getMoveFromMouseInput();
	void outputBoard(const ttt::Board& board);

	Ai* m_ai;
	int m_playerCount;
	ttt::PlayerColor m_currentPlayer = ttt::PlayerColor::Cross;
	ttt::PlayerColor m_aiColor;
	ttt::Board m_board;
	bool m_quit = false;
	std::unique_ptr<ttt::Renderer> m_renderer;
	Mouse m_mouse = Mouse();
};


#endif //DEEPREINFORCEMENTLEARNING_TICTACTOE_H

#include "ConnectFour/ConnectFour.h"

using namespace cn4;

ConnectFour::ConnectFour()
{
	m_currentPlayerColor = PlayerColor::YELLOW;
	m_playerCount = 2;
	m_renderer = std::make_unique<Renderer>();
	m_board = Board();
}

ConnectFour::ConnectFour(cn4::PlayerColor aiColor, Ai* ai)
{
	m_currentPlayerColor = cn4::PlayerColor::YELLOW;
	m_playerCount = 1;
	m_aiColor = aiColor;
	m_ai = ai;
	m_renderer = std::make_unique<Renderer>();
	m_board = Board();
}

void ConnectFour::gameLoop()
{
	while (!m_renderer->isQuit()) 
	{
		m_renderer->update(m_board);

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if(!m_gameOver)
			updateGame();
	}
	m_renderer->quit();
}

void ConnectFour::updateGame()
{
	if (m_playerCount == 1)
		update1PlayerGame();
	else
		update2PlayerGame();
}

bool ConnectFour::isValidInput(const cn4::Board& board, int action)
{
	if (action < 0 || action >= cn4::boardWidth)
		return false;
	if (!isMovePossible(board, action))
		return false;

	return true;
}

void ConnectFour::update1PlayerGame()
{
	if (m_currentPlayerColor == m_aiColor)
		updateAiMove();
	else
		updateHumanMove();
}

void ConnectFour::update2PlayerGame()
{
	updateHumanMove();
}

void ConnectFour::updateAiMove()
{
	int action = m_ai->getMove(m_board.toString(), static_cast<int>(m_currentPlayerColor));
	m_board.makeMove(action, m_currentPlayerColor);
	if (isGameOver(m_board))
		m_gameOver = true;
	else
		m_currentPlayerColor = getNextPlayer(m_currentPlayerColor);
}

void ConnectFour::updateHumanMove()
{
	int action = getActionFromMouseInput();
	if (!isValidInput(m_board, action))
		return;

	m_board.makeMove(action, m_currentPlayerColor);
	if (isGameOver(m_board))
		m_gameOver = true;
	else
		m_currentPlayerColor = getNextPlayer(m_currentPlayerColor);
}

int ConnectFour::getActionFromMouseInput()
{
	int action = -1;
	m_mouse.update();

	if (!m_mouse.isNewLeftClick())
		return action;

	int x = m_mouse.getMousePositionX();
	action = x / (m_renderer->windowWidth() / boardWidth);

	return action;
}

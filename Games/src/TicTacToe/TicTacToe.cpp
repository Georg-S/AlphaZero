#include "TicTacToe/TicTacToe.h"

using namespace ttt;

TicTacToe::TicTacToe()
{
	m_playerCount = 2;
	m_renderer = std::make_unique<Renderer>();
}

TicTacToe::TicTacToe(Ai* ai, PlayerColor aiColor)
{
	m_playerCount = 1;
	m_ai = ai;
	m_aiColor = aiColor;
	m_renderer = std::make_unique<Renderer>();
}

void TicTacToe::gameLoop()
{
	outputBoard(m_board);
	while (!m_quit)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		updateGame();
	}

	bool renderingClosed = false;
	while (!renderingClosed)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		m_renderer->updateQuit();
		if (m_renderer->isQuit())
		{
			m_renderer->quit();
			renderingClosed = true;
		}
	}
}

void TicTacToe::updateGame()
{
	m_renderer->updateQuit();
	if (m_renderer->isQuit())
	{
		m_quit = true;
		return;
	}

	if (m_playerCount == 1)
		updateOnePlayerGame();
	else
		updateTwoPlayerGame();
}

void TicTacToe::updateOnePlayerGame()
{
	if (m_currentPlayer == m_aiColor)
		updateAiMove(m_currentPlayer, m_board);
	else
		updateHumanPlayerMove(m_currentPlayer, m_board);
}

void TicTacToe::updateTwoPlayerGame()
{
	updateHumanPlayerMove(m_currentPlayer, m_board);
}

void TicTacToe::updateAiMove(PlayerColor currentPlayer, Board& board)
{
	int act = m_ai->getMove(board.toString(), static_cast<int>(m_aiColor));
	Move move = act;
	board.makeMove(move, currentPlayer);
	outputBoard(board);

	if (isGameOver(board))
		handleGameOver();
	else
		m_currentPlayer = getNextPlayer(currentPlayer);
}

void TicTacToe::updateHumanPlayerMove(PlayerColor currentPlayer, Board& board)
{
	Move move = getMoveFromMouseInput();
	if (!isValidMove(move, board))
		return;

	board.makeMove(move, currentPlayer);
	outputBoard(board);
	if (isGameOver(board))
		handleGameOver();
	else
		m_currentPlayer = getNextPlayer(currentPlayer);
}

ttt::Move TicTacToe::getMoveFromMouseInput()
{
	m_mouse.update();

	if (!m_mouse.isNewLeftClick())
		return Move(-1, -1);

	int x = m_mouse.getMousePositionX();
	int y = m_mouse.getMousePositionY();
	int moveX = x / (m_renderer->windowWidth() / 3);
	int moveY = y / (m_renderer->windowHeight() / 3);

	return Move(moveX, moveY);
}

void TicTacToe::handleGameOver()
{
	outputBoard(m_board);
	m_quit = true;
}

bool TicTacToe::isValid(const Move& move)
{
	if (move.x < 0 || move.x > 2)
		return false;
	if (move.y < 0 || move.y > 2)
		return false;

	return true;
}

void TicTacToe::outputBoard(const ttt::Board& board)
{
	m_renderer->renderBoard(board);
}

bool TicTacToe::isValidMove(const Move& move, const Board& board)
{
	if (!isValid(move))
		return false;
	if (!board.isMovePossible(move))
		return false;

	return true;
}
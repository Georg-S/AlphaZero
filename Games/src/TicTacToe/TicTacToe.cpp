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
	while (!m_renderer->isQuit())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		m_renderer->update(m_board);
		if(!m_gameOver)
			updateGame();
	}

	m_renderer->quit();
}

void TicTacToe::updateGame()
{
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

	auto [x, y] = m_mouse.getMousePosition();
	int moveX = x / (m_renderer->windowWidth() / 3);
	int moveY = y / (m_renderer->windowHeight() / 3);

	return Move(moveX, moveY);
}

void TicTacToe::handleGameOver()
{
	m_gameOver = true;
}

bool TicTacToe::isValid(const Move& move)
{
	if (move.x < 0 || move.x > 2)
		return false;
	if (move.y < 0 || move.y > 2)
		return false;

	return true;
}

bool TicTacToe::isValidMove(const Move& move, const Board& board)
{
	if (!isValid(move))
		return false;
	if (!board.isMovePossible(move))
		return false;

	return true;
}
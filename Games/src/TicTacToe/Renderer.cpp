#include "TicTacToe/Renderer.h"

using namespace ttt;

ttt::Renderer::Renderer()
{
	m_sdlHandler = std::make_unique<SDLHandler>(m_windowWidth, m_windowHeight, true);
	m_sdlHandler->start("Tic-Tac-Toe");
}

void ttt::Renderer::renderBoard(const Board& board)
{
	m_sdlHandler->clear();
	m_sdlHandler->createAndPushBackRenderElement("Images/TicTacToe/Board.png", 0, 0, m_windowWidth, m_windowHeight);

	auto boardStr = board.toString();
	assert(boardStr.size() == 9);

	for (int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 3; y++)
		{
			char c = boardStr.at(x + y * 3);
			if (c == 'X')
				m_sdlHandler->createAndPushBackRenderElement("Images/TicTacToe/Cross.png", x * pieceWidth, y * pieceWidth, pieceWidth, pieceHeight);
			else if (c == 'O')
				m_sdlHandler->createAndPushBackRenderElement("Images/TicTacToe/Dot.png", x * pieceWidth, y * pieceWidth, pieceWidth, pieceHeight);
		}
	}

	m_sdlHandler->update();
}

void ttt::Renderer::updateQuit()
{
	m_sdlHandler->update();
}

void ttt::Renderer::quit()
{
	m_sdlHandler->close();
}

bool ttt::Renderer::isQuit() const
{
	return m_sdlHandler->isExit();
}

int ttt::Renderer::windowWidth() const
{
	return m_windowWidth;
}

int ttt::Renderer::windowHeight() const
{
	return m_windowHeight;
}

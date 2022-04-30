#include "ConnectFour/Renderer.h"

using namespace cn4;

cn4::Renderer::Renderer()
{
	// If caching = false is used, a call too handler->clear would be needed before destruction to avoid memory leaks
	handler = std::make_unique<SDLHandler>(m_windowWidth, m_windowHeight, true);
	handler->start("Connect-Four");
}

void cn4::Renderer::update(const Board& board)
{
	handler->clear();

	for (int x = 0; x < boardWidth; x++)
	{
		for (int y = 0; y < boardHeight; y++)
		{
			if (board.at(x,y) != PlayerColor::NONE)
				renderPiece(board, x, y);
		}
	}

	handler->createAndPushBackRenderElement("Images/ConnectFour/Board.png", 0, 0, m_windowWidth, m_windowHeight);
	handler->update();
}

void cn4::Renderer::renderPiece(const Board& board, int x, int y)
{
	int xRend = convertBoardXPositionToRenderXPosition(board, x);
	int yRend = convertBoardYPositionToRenderYPosition(board, y);
	int pieceWidth = m_windowWidth / boardWidth;
	const std::string basePath = "Images/ConnectFour/";

	if (board.at(x,y) == PlayerColor::YELLOW)
		handler->createAndPushBackRenderElement(basePath + "YellowPiece.png", xRend, yRend, pieceWidth, pieceWidth);
	else
		handler->createAndPushBackRenderElement(basePath + "RedPiece.png", xRend, yRend, pieceWidth, pieceWidth);
}

int cn4::Renderer::convertBoardXPositionToRenderXPosition(const cn4::Board& board, int x)
{
	int result = (m_windowWidth / boardWidth) * x;

	return result;
}

int cn4::Renderer::convertBoardYPositionToRenderYPosition(const cn4::Board& board, int y)
{
	int result = (m_windowHeight / boardHeight) * y;
	result = m_windowHeight - m_windowHeight / boardHeight - result;

	return result;
}

void cn4::Renderer::quit()
{
	handler->close();
}

bool cn4::Renderer::isQuit() const
{
	return handler->isExit();
}

int cn4::Renderer::getWindowXPosition() const
{
	auto [x, y] = handler->getWindowPosition();

	return x;
}

int cn4::Renderer::getWindowYPosition() const
{
	auto [x, y] = handler->getWindowPosition();

	return y;
}

int cn4::Renderer::windowWidth() const
{
	return m_windowWidth;
}

int cn4::Renderer::windowHeight() const
{
	return m_windowHeight;
}

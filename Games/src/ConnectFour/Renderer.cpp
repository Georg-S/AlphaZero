#include "ConnectFour/Renderer.h"

cn4::Renderer::Renderer()
{
	handler = new SDLHandler(windowWidth, windowHeight, true);
	handler->start("Connect-Four");
}

cn4::Renderer::~Renderer()
{
	handler->clear();
	delete handler;
}

void cn4::Renderer::renderBoard(cn4::Board board)
{
	handler->clear();

	for (int x = 0; x < board.width; x++)
	{
		for (int y = 0; y < board.height; y++)
		{
			if (board.board[x][y] != (int)cn4::PlayerColor::NONE)
				renderPiece(board, x, y);
		}
	}

	handler->createAndPushBackRenderElement("Images/ConnectFour/Board.png", 0, 0, windowWidth, windowHeight);
	handler->update();
}

void cn4::Renderer::renderPiece(const cn4::Board& board, int x, int y)
{
	int xRend = convertBoardXPositionToRenderXPosition(board, x);
	int yRend = convertBoardYPositionToRenderYPosition(board, y);
	int pieceWidth = windowWidth / board.width;

	if (board.board[x][y] == (int)cn4::PlayerColor::YELLOW)
	{
		handler->createAndPushBackRenderElement("Images/ConnectFour/YellowPiece.png", xRend, yRend, pieceWidth, pieceWidth);
	}
	else
	{
		handler->createAndPushBackRenderElement("Images/ConnectFour/RedPiece.png", xRend, yRend, pieceWidth, pieceWidth);
	}
}

int cn4::Renderer::convertBoardXPositionToRenderXPosition(const cn4::Board& board, int x)
{
	int result = (windowWidth / board.width) * x;

	return result;
}

int cn4::Renderer::convertBoardYPositionToRenderYPosition(const cn4::Board& board, int y)
{
	int result = (windowHeight / board.height) * y;
	result = windowHeight - windowHeight / board.height - result;

	return result;
}

int cn4::Renderer::getWindowXPosition()
{
	auto [x, y] = handler->getWindowPosition();

	return x;
}

int cn4::Renderer::getWindowYPosition()
{
	auto [x, y] = handler->getWindowPosition();

	return y;
}

bool cn4::Renderer::isQuit()
{
	return handler->isExit();
}

void cn4::Renderer::quit()
{
	handler->close();
}

void cn4::Renderer::updateQuit()
{
	handler->update();
}

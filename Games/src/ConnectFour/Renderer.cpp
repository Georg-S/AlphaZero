#include "ConnectFour/Renderer.h"

cn4::Renderer::Renderer() {
	handler = new SDLHandler(windowWidth, windowHeight);
	handler->start("Connect-Four");
}

cn4::Renderer::~Renderer() {
	handler->clear();
	delete handler;
}

void cn4::Renderer::renderBoard(cn4::Board board) {
	handler->clear();
	handler->createAndPushBackRenderElement("Images/ConnectFour/Board.png", 0, 0, windowWidth, windowHeight);

	for (int x = 0; x < board.width; x++) {
		for (int y = 0; y < board.height; y++) {
			if (board.board[x][y] != cn4::PlayerColor::NONE)
				renderPiece(board, x, y);
		}
	}
	handler->updateRendering();
}

void cn4::Renderer::renderPiece(const cn4::Board& board, int x, int y) {
	int xRend = convertBoardXPositionToRenderXPosition(board, x);
	int yRend = convertBoardYPositionToRenderYPosition(board, y);
	int pieceWidth = windowWidth / board.width;

	if (board.board[x][y] == cn4::PlayerColor::YELLOW) {
		handler->createAndPushFrontRenderElement("Images/ConnectFour/YellowPiece.png", xRend, yRend, pieceWidth, pieceWidth);
	}
	else {
		handler->createAndPushFrontRenderElement("Images/ConnectFour/RedPiece.png", xRend, yRend, pieceWidth, pieceWidth);
	}
}

int cn4::Renderer::convertBoardXPositionToRenderXPosition(const cn4::Board& board, int x) {
	int result = (windowWidth / board.width) * x;

	return result;
}

int cn4::Renderer::convertBoardYPositionToRenderYPosition(const cn4::Board& board, int y) {
	int result = (windowHeight / board.height) * y;
	result = windowHeight - windowHeight / board.height - result;

	return result;
}

int cn4::Renderer::getWindowXPosition() {
	int x, y;
	handler->getWindowPosition(&x, &y);

	return x;
}

int cn4::Renderer::getWindowYPosition() {
	int x, y;
	handler->getWindowPosition(&x, &y);

	return y;
}

bool cn4::Renderer::isQuit() {
	return handler->exit;
}

void cn4::Renderer::quit() {
	handler->close();
}

void cn4::Renderer::updateQuit() {
	handler->updateQuit();
}

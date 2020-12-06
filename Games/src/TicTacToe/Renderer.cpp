#include "TicTacToe/Renderer.h"

ttt::Renderer::Renderer() {
	sdlHandler = new SDLHandler(windowWidth, windowHeight);
	pieceWidth = windowWidth / 3;
	pieceHeight = windowHeight / 3;

	sdlHandler->start("Tic-Tac-Toe");
}

ttt::Renderer::~Renderer() {
	delete sdlHandler;
}

void ttt::Renderer::renderBoard(const ttt::Board& board) {
	sdlHandler->clear();
	sdlHandler->createAndPushBackRenderElement("Images/TicTacToe/Board.png", 0, 0, windowWidth, windowHeight);

	for (int x = 0; x < 3; x++) {
		for (int y = 0; y < 3; y++) {
			if (board.board[x][y] != 0) {
				if (board.board[x][y] == 1)
					sdlHandler->createAndPushBackRenderElement("Images/TicTacToe/Cross.png", x * pieceWidth, y * pieceWidth, pieceWidth, pieceHeight);
				else
					sdlHandler->createAndPushBackRenderElement("Images/TicTacToe/Dot.png", x * pieceWidth, y * pieceWidth, pieceWidth, pieceHeight);
			}
		}
	}

	sdlHandler->updateRendering();
}

void ttt::Renderer::updateQuit() {
	sdlHandler->updateQuit();
}

void ttt::Renderer::quit() {
	sdlHandler->close();
}

bool ttt::Renderer::isQuit() {
	return sdlHandler->exit;
}

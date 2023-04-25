#include "Test_Utility.h"

static ceg::ChessEngine chessEngine = ceg::ChessEngine();

void renderBoard(const ttt::Board& board) 
{
	ttt::Renderer renderer = ttt::Renderer();
	while (!renderer.isQuit())
		renderer.update(board);

	renderer.quit();
}

void renderBoard(const cn4::Board& board) 
{
	cn4::Renderer renderer = cn4::Renderer();
	while (!renderer.isQuit())
		renderer.update(board);

	renderer.quit();
}

void renderBoard(const ceg::BitBoard& board)
{
	chess::Renderer renderer = chess::Renderer(&chessEngine);
	while (!renderer.is_quit())
		renderer.render_board(board);

	renderer.quit();
}

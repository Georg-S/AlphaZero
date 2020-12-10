#include "Chess/Renderer.h"

chess::Renderer::Renderer() {
	this->sdlHandler = new SDLHandler(windowWidth, windowHeight, true);
	pieceWidth = windowWidth / 8;
	pieceHeight = windowHeight / 8;
	this->sdlHandler->start("Chess");
}

chess::Renderer::~Renderer() {
	delete sdlHandler;
}

void chess::Renderer::render(const chess::RenderInformation& renderInfo) {
	sdlHandler->clear();
	renderChessBoard();

	if (renderInfo.previousMove.fromX != -1)
		renderPreviousMove(renderInfo.previousMove);

	if (renderInfo.selectedPieceY == -1 || renderInfo.selectedPieceX == -1)
		renderPieces(renderInfo.board);
	else
		renderPiecesWithSelectedOnMousePosition(renderInfo);


	sdlHandler->updateRendering();
}

void chess::Renderer::renderChessBoard()
{
	sdlHandler->createAndPushBackRenderElement("Images/Chess/Board.png", 0, 0, windowWidth, windowHeight);
}

void chess::Renderer::renderPieces(const chess::Board& board) {
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			if (board.board[x][y] != nullptr)
				renderPiece(board, x, y);
		}
	}
}

void chess::Renderer::renderPiecesWithSelectedOnMousePosition(const chess::RenderInformation& renderInfo) {
	Piece* foreGroundPiece = nullptr;

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			if (x == renderInfo.selectedPieceX && y == renderInfo.selectedPieceY)
				foreGroundPiece = renderInfo.board.board[x][y];
			else if (renderInfo.board.board[x][y] != nullptr)
				renderPiece(renderInfo.board, x, y);
		}
	}
	renderPieceOnMousePosition(foreGroundPiece, renderInfo.mousePositionX, renderInfo.mousePositionY);
}

void chess::Renderer::renderPreviousMove(const Move& previousMove)
{
	sdlHandler->createAndPushBackRenderElement("Images/Chess/PreviousMove.png", pieceWidth * previousMove.fromX, pieceHeight * previousMove.fromY
		, pieceWidth, pieceHeight);

	sdlHandler->createAndPushBackRenderElement("Images/Chess/PreviousMove.png", pieceWidth * previousMove.toX, pieceHeight * previousMove.toY
		, pieceWidth, pieceHeight);
}

void chess::Renderer::renderPiece(const chess::Board& board, int x, int y) {
	std::string fileString = getFileString(board.board[x][y]);
	sdlHandler->createAndPushBackRenderElement(fileString, pieceWidth * x, pieceHeight * y, pieceWidth, pieceHeight);
}

std::string chess::Renderer::getFileString(chess::Piece* piece) {
	chess::PieceColor pieceColor = piece->getPieceColor();
	char pieceChar = piece->getPieceChar();
	return getFileString(pieceChar, pieceColor);
}

std::string chess::Renderer::getFileString(const char& pieceChar, const chess::PieceColor& pieceColor) {
	std::string color;
	std::string pieceStr;
	if (pieceColor == chess::PieceColor::WHITE)
		color = "white";
	else if (pieceColor == chess::PieceColor::BLACK)
		color = "black";


	switch (pieceChar) {
	case 'P':
		pieceStr = "Pawn";
		break;
	case 'H':
		pieceStr = "Knight";
		break;
	case 'Q':
		pieceStr = "Queen";
		break;
	case 'K':
		pieceStr = "King";
		break;
	case 'B':
		pieceStr = "Bishop";
		break;
	case 'R':
		pieceStr = "Rook";
		break;
	default:
		break;
	}

	return "Images/Chess/" + pieceStr + "_" + color + ".png";
}

int chess::Renderer::getWindowWidth() {
	return this->windowWidth;
}

int chess::Renderer::getWindowHeight() {
	return this->windowHeight;
}

void chess::Renderer::renderPieceOnMousePosition(chess::Piece* piece, int mouseX, int mouseY) {
	std::string fileString = getFileString(piece);
	sdlHandler->createAndPushBackRenderElement(fileString, mouseX - (pieceWidth / 2), mouseY - (pieceHeight / 2), pieceWidth, pieceHeight);
}

void chess::Renderer::renderPromotionSelection(chess::PieceColor color) {
	sdlHandler->clear();
	sdlHandler->createAndPushBackRenderElement("Images/Chess/background.png", 0, 0, windowWidth, windowHeight);

	std::string queenStr = getFileString('Q', color);
	std::string rookStr = getFileString('R', color);
	std::string knightStr = getFileString('H', color);
	std::string  bishopStr = getFileString('B', color);

	sdlHandler->createAndPushBackRenderElement(queenStr, 0, 0, windowWidth / 2, windowHeight / 2);
	sdlHandler->createAndPushBackRenderElement(rookStr, windowWidth / 2, 0, windowWidth / 2, windowHeight / 2);
	sdlHandler->createAndPushBackRenderElement(knightStr, 0, windowHeight / 2, windowWidth / 2, windowHeight / 2);
	sdlHandler->createAndPushBackRenderElement(bishopStr, windowWidth / 2, windowHeight / 2, windowWidth / 2, windowHeight / 2);

	sdlHandler->updateRendering();
}

void chess::Renderer::updateQuit() {
	sdlHandler->updateQuit();
}

bool chess::Renderer::isQuit() {
	return sdlHandler->exit;
}

void chess::Renderer::quit() {
	sdlHandler->close();
}

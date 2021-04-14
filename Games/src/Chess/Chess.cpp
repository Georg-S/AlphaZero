#include "Chess/Chess.h"

Chess::Chess() 
{
	board.setToInitialState();
	renderer = new chess::Renderer();
	mouse = Mouse();
	windowWidth = renderer->getWindowWidth();
	windowHeight = renderer->getWindowHeight();
	playerCount = 2;
}

Chess::Chess(chess::PieceColor playerColor, Ai* ai) 
{
	board.setToInitialState();
	renderer = new chess::Renderer();
	mouse = Mouse();
	windowWidth = renderer->getWindowWidth();
	windowHeight = renderer->getWindowHeight();

	this->ai = ai;
	this->aiColor = chess::GameLogic::getNextPlayer(playerColor);
	playerCount = 1;
}

void Chess::gameLoop() 
{
	outputBoard();
	while (!gameOver) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		updateGame();
	}

	bool renderingClosed = false;
	while (!renderingClosed) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		renderer->updateQuit();
		if (renderer->isQuit()) {
			renderer->quit();
			renderingClosed = true;
			delete renderer;
		}
	}
}

void Chess::updateGame() 
{
	renderer->updateQuit();
	if (renderer->isQuit()) {
		gameOver = true;
		return;
	}

	if (playerCount == 1)
		update1PlayerGame();
	else
		update2PlayerGame();
}

void Chess::update1PlayerGame() 
{
	if (currentPlayer == aiColor)
		updateAiMove();
	else
		updateHumanMove();
}

void Chess::updateAiMove() 
{
	int intMove = ai->getMove(ReducedChessAdapter::convertStateToString(board, (int)currentPlayer), (int)currentPlayer);
	chess::Move move = chess::Move(intMove);
	chess::GameLogic::makeMove(board, move);
	previousMove = move;
	outputBoard();
	currentPlayer = chess::GameLogic::getNextPlayer(currentPlayer);

	if (chess::GameLogic::isGameOver(board, currentPlayer))
		handleGameOver();
}

void Chess::update2PlayerGame() 
{
	updateHumanMove();
}

void Chess::updateHumanMove() 
{
	chess::Move move = getMove();
	if (!isValidMove(move))
		return;

	chess::GameLogic::makeMove(board, move);
	previousMove = move;
	outputBoard();

	if (chess::GameLogic::pawnReachedEndOfBoard(board)) {
		handlePromoSelection(board, move.toX, move.toY);
	}
	currentPlayer = chess::GameLogic::getNextPlayer(currentPlayer);

	if (chess::GameLogic::isGameOver(board, currentPlayer))
		handleGameOver();
}

void Chess::outputBoard() 
{
	if (graphicalOutput)
		renderer->render(chess::RenderInformation(board, previousMove));
	else
		printBoardToConsole(board);
}

void Chess::printBoardToConsole(const chess::Board& board) 
{

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (chess::GameLogic::isFieldEmpty(board, x, y))
				std::cout << "-" << "  ";
			else
				std::cout << board.board[x][y]->getPieceChar() << "  ";
		}
		std::cout << std::endl;
	}
}

chess::Move Chess::getMove() {
	if (graphicalOutput)
		return getMoveFromGraphicalInput();
	else
		return getMoveFromKeyboardInput();
}

chess::Move Chess::getMoveFromKeyboardInput() 
{
	bool validMoveChosen = false;
	chess::Move move;

	while (!validMoveChosen) {
		std::cout << "Choose fromX (0-7)" << std::endl;
		std::cin >> move.fromX;
		std::cout << "Choose fromY (0-7)" << std::endl;
		std::cin >> move.fromY;
		std::cout << "Choose toX (0-7)" << std::endl;
		std::cin >> move.toX;
		std::cout << "Choose toY (0-7)" << std::endl;
		std::cin >> move.toY;

		if (isValidMove(move))
			validMoveChosen = true;
	}
	return move;
}

chess::Move Chess::getMoveFromGraphicalInput() 
{
	mouse.update();
	chess::Move move = chess::Move(-1, -1, -1, -1);
	chess::RenderInformation renderInfo = chess::RenderInformation();
	renderInfo.board = board;

	if (pieceSelected) {
		renderInfo.selectedPieceX = selectedPieceX;
		renderInfo.selectedPieceY = selectedPieceY;
		renderInfo.mousePositionX = mouse.getMousePositionX();
		renderInfo.mousePositionY = mouse.getMousePositionY();
		renderInfo.previousMove = previousMove;
		renderer->render(renderInfo);
	}

	if (mouse.isRightPressed()) {
		pieceSelected = false;
		selectedPieceX = -1;
		selectedPieceY = -1;
		outputBoard();
		return move;
	}

	if (!mouse.isNewLeftClick())
		return move;

	int boardX = convertMousePositionXToBoardPosition(mouse.getMousePositionX());
	int boardY = convertMousePositionYToBoardPosition(mouse.getMousePositionY());

	if (isOutOfRange(boardX) || isOutOfRange(boardY))
		return move;

	if (!pieceSelected) {
		if ((board.board[boardX][boardY] != nullptr) && (board.board[boardX][boardY]->getPieceColor() == currentPlayer)) {
			pieceSelected = true;
			selectedPieceX = boardX;
			selectedPieceY = boardY;
		}
	}
	else {
		int toX = boardX;
		int toY = boardY;

		move = chess::Move(selectedPieceX, selectedPieceY, toX, toY);
		selectedPieceX = -1;
		selectedPieceY = -1;
		pieceSelected = false;
		if (!chess::GameLogic::isMoveValid(board, move))
			outputBoard();
	}

	return move;
}

bool Chess::isValidMove(const chess::Move& move) 
{
	if (move.fromX < 0 || move.fromY < 0 || move.toX < 0 || move.toY < 0)
		return false;
	if (move.fromX > 7 || move.fromY > 7 || move.toX > 7 || move.toY > 7)
		return false;
	if (board.board[move.fromX][move.fromY] == nullptr)
		return false;

	return chess::GameLogic::isMoveValid(board, move);
}

void Chess::handleGameOver() 
{
	outputBoard();
	gameOver = true;
}

int Chess::convertMousePositionXToBoardPosition(int mouseX) {
	return mouseX / (windowWidth / 8);
}



int Chess::convertMousePositionYToBoardPosition(int mouseY) 
{
	return mouseY / (windowHeight / 8);
}

bool Chess::isOutOfRange(int boardPosition) {
	if (boardPosition >= 8 || boardPosition < 0)
		return true;
	return false;
}

void Chess::handlePromoSelection(chess::Board& board, int pawnX, int pawnY) 
{
	renderer->renderPromotionSelection(currentPlayer);

	bool validPieceSelected = false;

	while (!validPieceSelected) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		mouse.update();

		if (!mouse.isNewLeftClick())
			continue;

		chess::Piece* piece = getPieceFromPromoSelection(mouse.getMousePositionX(), mouse.getMousePositionY());

		if (piece == nullptr)
			continue;

		delete board.board[pawnX][pawnY];
		board.board[pawnX][pawnY] = piece;
		validPieceSelected = true;
	}
	this->outputBoard();
}

chess::Piece* Chess::getPieceFromPromoSelection(int mouseX, int mouseY)
{
	int x = mouseX / (windowWidth / 2);
	int y = mouseY / (windowHeight / 2);

	if (x == 0 && y == 0)
		return new Queen(currentPlayer);
	else if (x == 1 && y == 0)
		return new Rook(currentPlayer);
	else if (x == 0 && y == 1)
		return new Knight(currentPlayer);
	else if (x == 1 && y == 1)
		return new Bishop(currentPlayer);

	return nullptr;
}

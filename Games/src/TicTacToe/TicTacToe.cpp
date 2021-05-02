#include "TicTacToe/TicTacToe.h"

TicTacToe::TicTacToe()
{
	this->playerCount = 2;
	renderer = new ttt::Renderer();
}

TicTacToe::TicTacToe(Ai* ai, int aiColor)
{
	this->playerCount = 1;
	this->ai = ai;
	this->aiColor = aiColor;
	renderer = new ttt::Renderer();
}

void TicTacToe::reset()
{
	board = ttt::Board();
	currentPlayer = 1;
}

void TicTacToe::gameLoop()
{
	outputBoard(board);
	while (!quit)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		updateGame();
	}

	bool renderingClosed = false;
	while (!renderingClosed)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		renderer->updateQuit();
		if (renderer->isQuit())
		{
			renderer->quit();
			renderingClosed = true;
			delete renderer;
		}
	}
}

void TicTacToe::updateGame()
{
	renderer->updateQuit();
	if (renderer->isQuit())
	{
		quit = true;
		return;
	}

	if (playerCount == 1)
		updateOnePlayerGame();
	else
		updateTwoPlayerGame();
}

void TicTacToe::updateOnePlayerGame()
{
	if (currentPlayer == aiColor)
		updateAiMove(currentPlayer, board);
	else
		updateHumanPlayerMove(currentPlayer, board);
}

void TicTacToe::updateTwoPlayerGame()
{
	updateHumanPlayerMove(currentPlayer, board);
}

void TicTacToe::updateAiMove(int& currentPlayer, ttt::Board& board)
{
	int act = ai->getMove(convertBoardToString(board), aiColor);
	ttt::Move move = ttt::Move(act % 3, act / 3);
	ttt::GameLogic::makeMove(board, currentPlayer, move);
	outputBoard(board);
	if (ttt::GameLogic::isGameOver(board))
		handleGameOver();
	else
		currentPlayer = ttt::GameLogic::getNextPlayer(currentPlayer);
}

void TicTacToe::updateHumanPlayerMove(int& currentPlayer, ttt::Board& board)
{
	ttt::Move move = getInput();
	if (!isValidMove(move, board))
		return;

	ttt::GameLogic::makeMove(board, currentPlayer, move);
	outputBoard(board);
	if (ttt::GameLogic::isGameOver(board))
		handleGameOver();
	else
	{
		currentPlayer = ttt::GameLogic::getNextPlayer(currentPlayer);
	}
}

ttt::Move TicTacToe::getMoveFromConsole()
{
	bool validMove = false;
	int x, y = -1;

	while (!validMove)
	{
		std::cout << "Player " << currentPlayer << " enter your Moves X Coordinate " << std::endl;
		std::cin >> x;
		std::cout << "Player " << currentPlayer << " enter your Moves Y Coordinate " << std::endl;
		std::cin >> y;
		validMove = isValid(x, y);
		if (!validMove)
			std::cout << "One or both move-coordinates are out of range " << std::endl;
	}

	return ttt::Move(x, y);
}

ttt::Move TicTacToe::getMoveFromMouseInput()
{
	ttt::Move move = ttt::Move(-1, -1);
	mouse.update();

	if (!mouse.isNewLeftClick())
		return move;

	int x = mouse.getMousePositionX();
	int y = mouse.getMousePositionY();
	int moveX = x / (renderer->windowWidth / 3);
	int moveY = y / (renderer->windowHeight / 3);

	move = ttt::Move(moveX, moveY);

	return move;
}

void TicTacToe::handleGameOver()
{
	outputBoard(board);
	quit = true;
}

bool TicTacToe::isValid(int x, int y)
{
	if (x < 0 || x > 2)
		return false;
	if (y < 0 || y > 2)
		return false;

	return true;
}



void TicTacToe::printBoardToConsole(const ttt::Board& board)
{
	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			std::cout << convertPlayerNumberToChar(board.board[x][y]) << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl << std::endl;
}

char TicTacToe::convertPlayerNumberToChar(int number)
{
	if (number == 0)
		return '-';
	else if (number == 1)
		return 'X';
	else if (number == 2)
		return 'O';
	return 'F';
}

std::string TicTacToe::convertBoardToString(const ttt::Board& board)
{
	std::string result = "";

	for (int y = 0; y < 3; y++)
		for (int x = 0; x < 3; x++)
			result += std::to_string(board.board[x][y]);

	return result;
}

void TicTacToe::outputBoard(const ttt::Board& board)
{
	if (graphicalOutput)
		renderer->renderBoard(board);
	else
		printBoardToConsole(board);
}

ttt::Move TicTacToe::getInput()
{
	if (graphicalOutput)
		return getMoveFromMouseInput();
	else
		return getMoveFromConsole();
}

void TicTacToe::outputGameWinnerToConsole(const ttt::Board& board, const int currentPlayer)
{
	if (ttt::GameLogic::playerWon(board, currentPlayer))
		std::cout << " Game Over Player " << currentPlayer << " Won" << std::endl;
	else
		std::cout << "Game Over DRAW " << std::endl;
}

bool TicTacToe::isValidMove(const ttt::Move& move, const ttt::Board& board)
{
	if (!isValid(move.x, move.y))
		return false;
	if (!ttt::GameLogic::isMovePossible(board, move))
		return false;

	return true;
}
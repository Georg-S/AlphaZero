#include "ConnectFour/ConnectFour.h"


ConnectFour::ConnectFour()
{
	this->currentPlayerColor = cn4::PlayerColor::YELLOW;
	this->playerCount = 2;
	this->renderer = new cn4::Renderer();
}

ConnectFour::ConnectFour(cn4::PlayerColor aiColor, Ai* ai)
{
	this->currentPlayerColor = cn4::PlayerColor::YELLOW;
	this->playerCount = 1;
	this->aiColor = aiColor;
	this->ai = ai;
	this->renderer = new cn4::Renderer();
}

void ConnectFour::gameLoop()
{
	outputBoard(board);
	while (!gameOver)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
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

void ConnectFour::updateGame()
{
	renderer->updateQuit();
	if (renderer->isQuit())
	{
		gameOver = true;
		return;
	}

	if (playerCount == 1)
		update1PlayerGame();
	else
		update2PlayerGame();
}

void ConnectFour::update1PlayerGame()
{
	if (currentPlayerColor == aiColor)
		updateAiMove();
	else
		updateHumanMove();
}

void ConnectFour::update2PlayerGame()
{
	updateHumanMove();
}

void ConnectFour::updateAiMove()
{
	int action = ai->getMove(board.toString(), (int)currentPlayerColor);
	cn4::GameLogic::makeMove(board, action, currentPlayerColor);
	outputBoard(board);
	if (cn4::GameLogic::isGameOver(board))
		handleGameOver(board);
	else
		currentPlayerColor = getNextPlayer(currentPlayerColor);
}

void ConnectFour::updateHumanMove()
{
	int action = getAction();
	if (!isValidInput(board, action))
		return;

	cn4::GameLogic::makeMove(board, action, currentPlayerColor);
	outputBoard(board);
	if (cn4::GameLogic::isGameOver(board))
		handleGameOver(board);
	else
		currentPlayerColor = getNextPlayer(currentPlayerColor);
}

int ConnectFour::getAction()
{
	if (useGraphicalRender)
		return getActionFromMouseInput();
	else
		return getActionFromKeyboard();
}

int ConnectFour::getActionFromMouseInput()
{
	int action = -1;
	mouse.update();

	if (!mouse.isNewLeftClick())
		return action;

	int x = mouse.getMousePositionX();
	action = x / (renderer->windowWidth / board.width);

	return action;
}

int ConnectFour::getActionFromKeyboard()
{
	int action = -1;
	std::cout << "Select Row (0-6) for your Move " << std::endl;
	while (action == -1)
	{
		int input = -1;
		std::cin >> input;
		if (isValidInput(board, input))
			action = input;
	}
	return action;
}

bool ConnectFour::isValidInput(const cn4::Board& board, int action)
{
	if (action < 0 || action >= board.width)
		return false;
	if (!cn4::GameLogic::isMovePossible(board, action))
		return false;

	return true;
}



void ConnectFour::printBoardToConsole(const cn4::Board& board)
{
	std::cout << std::endl << std::endl;
	for (int y = board.height - 1; y >= 0; y--)
	{
		for (int x = 0; x < board.width; x++)
		{
			std::cout << convertPlayerColorToChar((cn4::PlayerColor)board.board[x][y]) << "  ";
		}
		std::cout << std::endl;
	}
}

char ConnectFour::convertPlayerColorToChar(const cn4::PlayerColor& playerColor)
{
	if (playerColor == cn4::PlayerColor::RED)
		return 'X';
	if (playerColor == cn4::PlayerColor::YELLOW)
		return 'O';
	return '-';
}

void ConnectFour::handleGameOver(const cn4::Board& board)
{
	std::cout << "Game Over " << std::endl;
	gameOver = true;
}

cn4::PlayerColor ConnectFour::getNextPlayer(const cn4::PlayerColor& player)
{
	return (cn4::PlayerColor)((int)player % 2 + 1);
}

void ConnectFour::outputBoard(const cn4::Board& board)
{
	if (useGraphicalRender)
		renderer->renderBoard(board);
	else
		printBoardToConsole(board);
}

#include "Chess/ReducedChessAdapter.h"

ReducedChessAdapter::ReducedChessAdapter()
{

}

std::vector<int> ReducedChessAdapter::getAllPossibleMoves(const std::string& state, int currentPlayer)
{
	chess::Board board = convertStateStringToBoard(state);
	std::vector<int> resultVec;

	for (int fromX = 0; fromX < 8; fromX++)
	{
		for (int fromY = 0; fromY < 8; fromY++)
		{
			if (board.board[fromX][fromY] == nullptr)
				continue;
			if (board.board[fromX][fromY]->getPieceColor() != ((chess::PieceColor)currentPlayer))
				continue;

			for (int toX = 0; toX < 8; toX++)
			{
				for (int toY = 0; toY < 8; toY++)
				{
					chess::Move move = chess::Move(fromX, fromY, toX, toY);
					if (chess::GameLogic::isMoveValid(board, move))
						resultVec.push_back(move.getSingleIntRepresentation());
				}
			}
		}
	}

	board.deletePieces();
	return resultVec;
}

int ReducedChessAdapter::getInitialPlayer()
{
	return (int)chess::PieceColor::WHITE;
}

std::string ReducedChessAdapter::getInitialGameState()
{
	static const std::string initialState = "1RHBQKBHRPPPPPPPP--------------------------------PPPPPPPPRHBQKBHR2222222222222222--------------------------------1111111111111111";
	return initialState;
}

int ReducedChessAdapter::getPlayerWon(const std::string& state)
{
	chess::Board board = convertStateStringToBoard(state);
	int result = 0;

	if (chess::GameLogic::isCheckMate(board, chess::PieceColor::BLACK))
		result = (int)chess::PieceColor::BLACK;
	else if (chess::GameLogic::isCheckMate(board, chess::PieceColor::WHITE))
		result = (int)chess::PieceColor::WHITE;

	board.deletePieces();
	return result;
}

int ReducedChessAdapter::getNextPlayer(int currentPlayer)
{
	return (int)(chess::GameLogic::getNextPlayer((chess::PieceColor)currentPlayer));
}

std::string ReducedChessAdapter::makeMove(const std::string& state, int move, int currentPlayer)
{
	chess::Board board = convertStateStringToBoard(state);
	chess::Move chessMove = chess::Move(move);

	int currentPlayerFromState = atoi(state.substr(0, 1).c_str());
	if (currentPlayerFromState != currentPlayer)
		std::cout << "Here might be a Problem" << std::endl;


	chess::GameLogic::makeMove(board, chessMove);

	if (chess::GameLogic::pawnReachedEndOfBoard(board))
		replacePawnsWithQueens(board);

	std::string resultString = convertStateToString(board, getNextPlayer(currentPlayer));

	if (!bothKingsStillThere(board))
	{
		std::cout << "Move: " << move << std::endl;
		std::cout << "Before: " << state << std::endl;
		std::cout << "After: " << resultString << std::endl;
	}

	board.deletePieces();
	return resultString;
}

torch::Tensor ReducedChessAdapter::convertStateToNeuralNetInput(const std::string& state, int currentPlayer,
	torch::Device device)
{
	chess::Board board = convertStateStringToBoard(state);
	chess::PieceColor otherPlayer = (chess::PieceColor)getNextPlayer(currentPlayer);
	torch::Tensor result = torch::zeros({ 1,12,8,8 });

	convertPiecesToTensor(board, (chess::PieceColor)currentPlayer, result[0][0], 'P');
	convertPiecesToTensor(board, (chess::PieceColor)currentPlayer, result[0][1], 'R');
	convertPiecesToTensor(board, (chess::PieceColor)currentPlayer, result[0][2], 'H');
	convertPiecesToTensor(board, (chess::PieceColor)currentPlayer, result[0][3], 'B');
	convertPiecesToTensor(board, (chess::PieceColor)currentPlayer, result[0][4], 'Q');
	convertPiecesToTensor(board, (chess::PieceColor)currentPlayer, result[0][5], 'K');

	convertPiecesToTensor(board, (chess::PieceColor)otherPlayer, result[0][6], 'P');
	convertPiecesToTensor(board, (chess::PieceColor)otherPlayer, result[0][7], 'R');
	convertPiecesToTensor(board, (chess::PieceColor)otherPlayer, result[0][8], 'H');
	convertPiecesToTensor(board, (chess::PieceColor)otherPlayer, result[0][9], 'B');
	convertPiecesToTensor(board, (chess::PieceColor)otherPlayer, result[0][10], 'Q');
	convertPiecesToTensor(board, (chess::PieceColor)otherPlayer, result[0][11], 'K');

	result = result.to(device);
	board.deletePieces();
	return result;
}

bool ReducedChessAdapter::isGameOver(const std::string& state)
{
	chess::PieceColor currentPlayer = getCurrentPlayerFromState(state);
	chess::Board board = convertStateStringToBoard(state);
	bool result = chess::GameLogic::isGameOver(board, currentPlayer);
	board.deletePieces();

	return result;
}

int ReducedChessAdapter::gameOverReward(const std::string& state, int currentPlayer)
{
	chess::Board board = convertStateStringToBoard(state);
	int value = 0;
	chess::PieceColor otherPlayer = chess::GameLogic::getNextPlayer((chess::PieceColor)currentPlayer);

	if (chess::GameLogic::isCheckMate(board, (chess::PieceColor)currentPlayer))
		value = -1;
	else if (chess::GameLogic::isCheckMate(board, (chess::PieceColor)otherPlayer))
		value = 1;


	board.deletePieces();
	return value;
}

std::string ReducedChessAdapter::convertBoardToString(chess::Board& board)
{
	return convertPiecesToString(board) + convertColorToString(board);
}

std::string ReducedChessAdapter::convertPiecesToString(const chess::Board& board)
{
	std::string resultStr = "";

	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			if (board.board[x][y] == nullptr)
				resultStr += '-';
			else
				resultStr += board.board[x][y]->getPieceChar();
		}
	}
	return resultStr;
}

std::string ReducedChessAdapter::convertColorToString(const chess::Board& board)
{
	std::string resultStr = "";

	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			if (board.board[x][y] == nullptr)
				resultStr += '-';
			else
				resultStr += std::to_string((int)board.board[x][y]->getPieceColor());
		}
	}
	return resultStr;
}

chess::Board ReducedChessAdapter::convertStringToBoard(std::string state)
{
	chess::Board resultBoard = chess::Board();
	std::string pieces = state.substr(0, 64);
	std::string colors = state.substr(64);

	for (int iter = 0; iter < 64; iter++)
	{
		char pieceChar = pieces.at(iter);
		int iColor = colors.at(iter) - '0';
		chess::PieceColor color = (chess::PieceColor)iColor;

		if (pieceChar == '-')
			continue;
		else
		{
			int x = iter % 8;
			int y = iter / 8;
			resultBoard.board[x][y] = getPieceFromChar(pieceChar, color);

			if (resultBoard.board[x][y]->getPieceChar() == 'P')
				resultBoard.board[x][y]->moved = false;
			else
				resultBoard.board[x][y]->moved = true;
		}
	}

	return resultBoard;
}

chess::Piece* ReducedChessAdapter::getPieceFromChar(const char& pieceChar, const chess::PieceColor& color)
{
	switch (pieceChar)
	{
	case 'Q':
		return new Queen(color);
	case 'H':
		return new Knight(color);
	case 'P':
		return new Pawn(color);
	case 'R':
		return new Rook(color);
	case 'K':
		return new King(color);
	case 'B':
		return new Bishop(color);
	default:
		return nullptr;
	}
}

chess::PieceColor ReducedChessAdapter::getCurrentPlayerFromState(const std::string& state)
{
	return (chess::PieceColor)(state[0] - '0');
}


void ReducedChessAdapter::convertPiecesToTensor(chess::Board& board, const chess::PieceColor& color,
	at::Tensor destination, const char& pieceChar)
{
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			if (board.board[x][y] == nullptr)
				continue;
			if (board.board[x][y]->getPieceChar() != pieceChar)
				continue;
			if (board.board[x][y]->getPieceColor() == color)
				destination[x][y] = 1.f;
		}
	}
}

void ReducedChessAdapter::replacePawnsWithQueens(chess::Board& board)
{
	replacePawnsWithQueens(board, 0);
	replacePawnsWithQueens(board, 7);
}

void ReducedChessAdapter::replacePawnsWithQueens(chess::Board& board, int row)
{
	for (int x = 0; x < 8; x++)
	{
		if (board.board[x][row] == nullptr)
			continue;
		if (board.board[x][row]->getPieceChar() != 'P')
			continue;

		chess::PieceColor  pawnColor = board.board[x][row]->getPieceColor();
		delete board.board[x][row];
		board.board[x][row] = new Queen(pawnColor);
	}
}

bool ReducedChessAdapter::bothKingsStillThere(const chess::Board& board)
{
	int numberKings = 0;
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			if (board.board[x][y] == nullptr)
				continue;
			if (board.board[x][y]->getPieceChar() == 'K')
				numberKings++;
		}
	}

	if (numberKings == 2)
		return true;

	return false;
}

std::string ReducedChessAdapter::convertStateToString(chess::Board& board, int currentPlayer)
{
	return std::to_string(currentPlayer) + convertBoardToString(board);
}

chess::Board ReducedChessAdapter::convertStateStringToBoard(std::string state)
{
	state = state.substr(1);
	return convertStringToBoard(state);
}

int ReducedChessAdapter::getActionCount() const
{
	return this->actionCount;
}

#include "ConnectFour/GameLogic.h"

bool cn4::GameLogic::isGameOver(const cn4::Board& board) {
	if (isDraw(board))
		return true;
	if (fourInARowHorizontal(board) || fourInARowVertical(board) || fourInARowDiagonal(board))
		return true;

	return false;
}

cn4::PlayerColor cn4::GameLogic::getPlayerWon(const cn4::Board& board) {
	if (fourInARowHorizontal(board, (int)cn4::PlayerColor::YELLOW) 
		|| fourInARowVertical(board, (int)cn4::PlayerColor::YELLOW) 
		|| fourInARowDiagonal(board, (int)cn4::PlayerColor::YELLOW))
		return cn4::PlayerColor::YELLOW;
	if (fourInARowHorizontal(board, (int) cn4::PlayerColor::RED) 
		|| fourInARowVertical(board, (int)cn4::PlayerColor::RED) 
		|| fourInARowDiagonal(board, (int)cn4::PlayerColor::RED))
		return cn4::PlayerColor::RED;

	return cn4::PlayerColor::NONE;
}

bool cn4::GameLogic::fourInARowHorizontal(const cn4::Board& board, int player) {
	for (int x = 0; x < board.width; x++) {
		for (int y = 0; y < board.height; y++) {
			if (board.board[x][y] == (int)cn4::PlayerColor::NONE)
				break;
			if (fourInARowHorizontal(board, player, x, y))
				return true;
		}
	}
	return false;
}

bool cn4::GameLogic::fourInARowHorizontal(const cn4::Board& board, int player, int xPos, int yPos) {
	if (!((xPos + 3) < board.width))
		return false;
	if (player != -1 && player != board.board[xPos][yPos])
		return false;
	if (board.board[xPos][yPos] == (int)cn4::PlayerColor::NONE)
		return false;

	for (int x = xPos; x < xPos + 3; x++) {
		if (board.board[x][yPos] != board.board[x + 1][yPos])
			return false;
	}

	return true;
}

bool cn4::GameLogic::fourInARowVertical(const cn4::Board& board, int player) {
	for (int x = 0; x < board.width; x++) {
		for (int y = 0; y < board.height; y++) {
			if (board.board[x][y] == (int)cn4::PlayerColor::NONE)
				break;
			if (fourInARowVertical(board, player, x, y))
				return true;
		}
	}
	return false;
}

bool cn4::GameLogic::fourInARowVertical(const cn4::Board& board, int player, int xPos, int yPos) {
	if ((yPos + 3) >= board.height)
		return false;
	if ((player != -1) && (player != board.board[xPos][yPos]))
		return false;
	if (board.board[xPos][yPos] == (int)cn4::PlayerColor::NONE)
		return false;

	for (int y = yPos; y < yPos + 3; y++) {
		if (board.board[xPos][y] != board.board[xPos][y + 1])
			return false;
	}
	return true;
}

bool cn4::GameLogic::fourInARowDiagonal(const cn4::Board& board, int player) {
	for (int x = 0; x < board.width; x++) {
		for (int y = 0; y < board.height; y++) {
			if (fourInARowDiagonalUp(board, player, x, y))
				return true;
			if (fourInARowDiagonalDown(board, player, x, y))
				return true;
		}
	}
	return false;
}

bool cn4::GameLogic::fourInARowDiagonalUp(const cn4::Board& board, int player, int xPos, int yPos) {
	if ((yPos + 3) >= board.height || (xPos + 3) >= board.width)
		return false;
	if ((player != -1) && (player != board.board[xPos][yPos]))
		return false;
	if (board.board[xPos][yPos] == (int)cn4::PlayerColor::NONE)
		return false;

	for (int x = xPos, y = yPos; x < xPos + 3; x++, y++) {
		if (board.board[x][y] != board.board[x + 1][y + 1])
			return false;
	}
	return true;
}

bool cn4::GameLogic::fourInARowDiagonalDown(const cn4::Board& board, int player, int xPos, int yPos) {
	if ((yPos - 3) < 0 || (xPos + 3) >= board.width)
		return false;
	if ((player != -1) && (player != board.board[xPos][yPos]))
		return false;
	if (board.board[xPos][yPos] == (int)cn4::PlayerColor::NONE)
		return false;

	for (int x = xPos, y = yPos; x < xPos + 3; x++, y--) {
		if (board.board[x][y] != board.board[x + 1][y - 1])
			return false;
	}
	return true;
}

bool cn4::GameLogic::isDraw(const cn4::Board& board) {
	for (int y = 0; y < board.height; y++) {
		for (int x = 0; x < board.width; x++) {
			if (board.board[x][y] == (int)cn4::PlayerColor::NONE)
				return false;
		}
	}
	return true;
}

void cn4::GameLogic::makeMove(cn4::Board& board, int action, const cn4::PlayerColor& playerColor) {
	for (int i = 0; i < board.height; i++) {
		if (board.board[action][i] == 0) {
			board.board[action][i] = (int)playerColor;
			return;
		}
	}
}

bool cn4::GameLogic::isMovePossible(const cn4::Board& board, int action) {
	if (board.board[action][board.height - 1] == (int)cn4::PlayerColor::NONE)
		return true;
	return false;
}

#include "TicTacToe/GameLogic.h"


bool ttt::GameLogic::isBoardFull(const ttt::Board &board) {
    for(int x = 0; x < 3; x++){
        for(int y = 0; y < 3; y++){
            if(board.board[x][y] == 0)
                return false;
        }
    }
    return true;
}

bool ttt::GameLogic::isGameOver(const ttt::Board &board) {
    return (isThreeInARowHorizontal(board) || isThreeInARowVertical(board) || isThreeInARowDiagonal(board) ||
            ttt::GameLogic::isBoardFull(board));
}

int ttt::GameLogic::getPlayerWon(const ttt::Board &board) {
    if(playerWon(board, 1))
        return 1;
    if(playerWon(board, 2))
        return 2;
    return 0;
}

bool ttt::GameLogic::isThreeInARowHorizontal(const ttt::Board &board, const int &player) {
    return isThreeInARowHorizontal(board, 0, player) | isThreeInARowHorizontal(board, 1, player) | isThreeInARowHorizontal(board, 2, player);
}

bool ttt::GameLogic::isThreeInARowHorizontal(const ttt::Board &board, const int &y, const int &player) {
    if(player != -1 && board.board[0][y] != player)
        return false;
    if(board.board[0][y] == 0)
        return false;
    if(board.board[0][y] != board.board[1][y])
        return false;
    if(board.board[1][y] != board.board[2][y])
        return false;
    return true;
}

bool ttt::GameLogic::isThreeInARowVertical(const ttt::Board &board, const int &player) {
    return isThreeInARowVertical(board, 0, player) | isThreeInARowVertical(board, 1, player) | isThreeInARowVertical(board, 2, player);
}

bool ttt::GameLogic::isThreeInARowVertical(const ttt::Board &board, const int &x, const int &player) {
    if(player != -1 && board.board[x][0] != player)
        return false;
    if(board.board[x][0] == 0)
        return false;
    if(board.board[x][0] != board.board[x][1])
        return false;
    if(board.board[x][1] != board.board[x][2])
        return false;
    return true;
}

bool ttt::GameLogic::isThreeInARowDiagonal(const ttt::Board &board, const int &player) {
    if(isThreeInARowDiagonalLeftCorner(board, player) || isThreeInARowDiagonalRightCorner(board, player))
        return true;
    return false;
}

bool ttt::GameLogic::isThreeInARowDiagonalLeftCorner(const ttt::Board &board, const int &player) {
    if(player != -1 && board.board[0][0] != player)
        return false;
    if(board.board[0][0] != 0){
        if((board.board[0][0] == board.board[1][1]) && (board.board[1][1] == board.board[2][2]))
            return true;
    }
    return false;
}

bool ttt::GameLogic::isThreeInARowDiagonalRightCorner(const ttt::Board &board, const int &player) {
    if(player != -1 && board.board[2][0] != player)
        return false;
    if(board.board[2][0] != 0){
        if((board.board[2][0] == board.board[1][1]) && (board.board[1][1] == board.board[0][2]))
            return true;
    }
    return false;
}

bool ttt::GameLogic::playerWon(const ttt::Board &board, const int &player) {
    return isThreeInARowHorizontal(board, player) | isThreeInARowVertical(board, player) | isThreeInARowDiagonal(board, player);
}

int ttt::GameLogic::getNextPlayer(const int &currentPlayer) {
    return currentPlayer%2+1;
}

bool ttt::GameLogic::isMovePossible(const ttt::Board &board, const ttt::Move &move) {
    if(board.board[move.x][move.y] != 0)
        return false;

    return true;
}

void ttt::GameLogic::makeMove(ttt::Board &board, const int &currentPlayer, const ttt::Move &move) {
    board.board[move.x][move.y] = currentPlayer;
}

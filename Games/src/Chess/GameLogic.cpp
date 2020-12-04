#include "Chess/GameLogic.h"

bool chess::GameLogic::isFieldEmpty(const chess::Board &board, const int &x, const int &y) {
    if(board.board[x][y] == nullptr)
        return true;
    return false;
}

void chess::GameLogic::makeMove(chess::Board &board, const chess::Move &move) {
    board.board[move.fromX][move.fromY]->makeMove(board.board, move);
}

bool chess::GameLogic::isGameOver(chess::Board &board) {
    if(isCheckMate(board, chess::PieceColor::BLACK))
        return true;
    if(isCheckMate(board, chess::PieceColor::WHITE))
        return true;
    if(isStaleMate(board, chess::PieceColor::BLACK))
        return true;
    if(isStaleMate(board, chess::PieceColor::WHITE))
        return true;

    return false;
}

chess::PieceColor chess::GameLogic::getNextPlayer(const chess::PieceColor color) {
    if(color == chess::PieceColor::WHITE)
        return chess::PieceColor ::BLACK;
    else if(color == chess::PieceColor::BLACK)
        return chess::PieceColor ::WHITE;
    return chess::PieceColor::NONE;
}

bool chess::GameLogic::isMoveValid(chess::Board &board, const chess::Move &move) {
    if(!board.board[move.fromX][move.fromY]->isMoveValid(board.board,move))
        return false;

    Board buf = Board(board);
    makeMove(buf, move);
    chess::PieceColor color = board.board[move.fromX][move.fromY]->getPieceColor();

    if(isCheck(buf, color)){
        buf.deletePieces();
        return false;
    }

    buf.deletePieces();
    return true;
}

bool chess::GameLogic::isCheck(chess::Board &board, const chess::PieceColor &pieceColor) {
    return chess::Piece::isCheck(board.board, pieceColor);
}

bool chess::GameLogic::isCheckMate(chess::Board &board, const chess::PieceColor &pieceColor) {
    if(!isCheck(board, pieceColor))
        return false;

    if(isAnyMovePossible(board, pieceColor))
        return false;

    return true;
}

bool chess::GameLogic::isStaleMate(chess::Board &board, const chess::PieceColor &pieceColor) {
    if(isCheck(board, pieceColor))
        return false;

    if(isAnyMovePossible(board, pieceColor))
        return false;

    return true;
}

bool chess::GameLogic::pawnReachedEndOfBoard(const chess::Board &board) {
    return isPawnInRow(board, 0) || isPawnInRow(board, 7);
}

bool chess::GameLogic::isPawnInRow(const chess::Board &board, const int &row) {
    for(int x = 0; x < 8 ; x++) {
        if(isFieldEmpty(board, x, row))
            continue;
        if(board.board[x][row]->getPieceChar() == 'P')
            return true;
    }
    return false;
}

bool chess::GameLogic::isAnyMovePossible(chess::Board &board, const chess::PieceColor &color) {
    for(int pieceX = 0; pieceX < 8; pieceX++) {
        for(int pieceY = 0; pieceY < 8; pieceY++) {
            if(board.board[pieceX][pieceY] == nullptr)
                continue;
            if(board.board[pieceX][pieceY]->getPieceColor() != color)
                continue;

            if(isAnyMoveWithPiecePossible(board, pieceX, pieceY))
                return true;
        }
    }
    return false;
}

bool chess::GameLogic::isAnyMoveWithPiecePossible(chess::Board &board, const int &pieceX, const int &pieceY) {
    for(int x = 0; x < 8; x++) {
        for(int y = 0; y < 8; y++) {
            if(isMoveValid(board, Move(pieceX, pieceY,x,y)))
                return true;
        }
    }

    return false;
}

#include "Chess/Pieces/Queen.h"

Queen::Queen(chess::PieceColor color) {
    this->color = color;
    this->pieceChar = 'Q';
    this->moved = false;
}

Queen::~Queen() {

}

bool Queen::isMoveValid(chess::Piece *(*board)[8], const chess::Move &move) {

    return diagonalPossible(board, move.fromX, move.fromY, move.toX, move.toY)
           || horizontalPossible(board, move.fromX, move.fromY, move.toX, move.toY)
           || verticalPossible(board, move.fromX, move.fromY, move.toX, move.toY);
}

void Queen::makeMove(chess::Piece *(*board)[8], const chess::Move &move) {
    setEnPassantDirectionsOfOneColorToZero(board, board[move.fromX][move.fromY]->getPieceColor());
    movePieceToPosition(board, move);
}

bool
Queen::rightPossible(chess::Piece *(*board)[8], const int &fromX, const int &fromY, const int &toX, const int &toY) {
    if (fromX >= toX)
        return false;

    for (int x = fromX + 1; x < 8; x++) {
        if (x == toX && fromY == toY) {
            if (isFieldEmpty(board, toX, toY))
                return true;

            if (piecesHaveDifferentColor(board, fromX, fromY, toX, toY))
                return true;

            return false;
        }
        if (!isFieldEmpty(board, x, fromY))
            return false;
    }
    return false;
}

bool Queen::upPossible(chess::Piece *(*board)[8], const int &fromX, const int &fromY, const int &toX, const int &toY) {
    for (int y = fromY -1; y >= 0; y--) {
        if (fromX == toX && y == toY) {
            if (isFieldEmpty(board, toX, toY))
                return true;

            if (piecesHaveDifferentColor(board, fromX, fromY, toX, toY))
                return true;

            return false;
        }
        if (!isFieldEmpty(board, fromX, y))
            return false;
    }
    return false;
}

bool
Queen::leftPossible(chess::Piece *(*board)[8], const int &fromX, const int &fromY, const int &toX, const int &toY) {
    if (fromX <= toX)
        return false;

    for (int x = fromX - 1; x >= 0; x--) {
        if (x == toX && fromY == toY) {
            if (isFieldEmpty(board, toX, toY))
                return true;

            if (piecesHaveDifferentColor(board, fromX, fromY, toX, toY))
                return true;

            return false;
        }
        if (!isFieldEmpty(board, x, fromY))
            return false;
    }

    return false;
}

bool
Queen::downPossible(chess::Piece *(*board)[8], const int &fromX, const int &fromY, const int &toX, const int &toY) {
    for (int y = fromY + 1; y < 8; y ++) {
        if (fromX == toX && y == toY) {
            if (isFieldEmpty(board, toX, toY))
                return true;

            if (piecesHaveDifferentColor(board, fromX, fromY, toX, toY))
                return true;

            return false;
        }
        if (!isFieldEmpty(board, fromX, y))
            return false;
    }
    return false;
}

bool
Queen::leftUpPossible(chess::Piece *(*board)[8], const int &fromX, const int &fromY, const int &toX, const int &toY) {
    for (int x = fromX-1, y = fromY-1; x >= 0 && y >= 0; x--, y--) {
        if (x == toX && y == toY ) {
            if (isFieldEmpty(board, toX, toY))
                return true;

            if (piecesHaveDifferentColor(board,fromX,fromY,toX,toY))
                return true;

            return false;
        }
        if (!isFieldEmpty(board, x, y))
            return false;
    }
    return false;
}

bool
Queen::leftDownPossible(chess::Piece *(*board)[8], const int &fromX, const int &fromY, const int &toX, const int &toY) {
    for (int x = fromX - 1, y = fromY + 1; x >= 0 && y < 8; x--, y++) {
        if (x == toX && y == toY) {
            if (isFieldEmpty(board, toX, toY))
                return true;

            if (piecesHaveDifferentColor(board, fromX, fromY, toX, toY))
                return true;

            return false;
        }
        if (!isFieldEmpty(board, x, y))
            return false;
    }
    return false;
}

bool Queen::rightDownPossible(chess::Piece *(*board)[8], const int &fromX, const int &fromY, const int &toX,
                              const int &toY) {
    for (int x = fromX + 1, y = fromY + 1; x < 8 && y < 8; x++, y++) {
        if (x == toX && y == toY) {
            if (isFieldEmpty(board, toX, toY))
                return true;

            if (piecesHaveDifferentColor(board, fromX, fromY, toX, toY))
                return true;

            return false;
        }
        if (!isFieldEmpty(board, x, y))
            return false;
    }
    return false;
}

bool
Queen::rightUpPossible(chess::Piece *(*board)[8], const int &fromX, const int &fromY, const int &toX, const int &toY) {
    for (int x = fromX + 1, y = fromY - 1; x < 8 && y >= 0; x++, y--) {
        if (x == toX && y == toY) {
            if (isFieldEmpty(board, toX, toY))
                return true;

            if (piecesHaveDifferentColor(board, fromX, fromY, toX, toY))
                return true;

            return false;
        }
        if (!isFieldEmpty(board, x, y))
            return false;
    }
    return false;
}

bool
Queen::diagonalPossible(chess::Piece *(*board)[8], const int &fromX, const int &fromY, const int &toX, const int &toY) {
    return leftUpPossible(board, fromX, fromY, toX, toY)
           || rightUpPossible(board, fromX, fromY, toX, toY)
           || leftDownPossible(board, fromX, fromY, toX, toY)
           || rightDownPossible(board, fromX, fromY, toX, toY);
}

bool Queen::horizontalPossible(chess::Piece *(*board)[8], const int &fromX, const int &fromY, const int &toX,
                               const int &toY) {
    return rightPossible(board, fromX, fromY, toX, toY)
           || leftPossible(board, fromX, fromY, toX, toY);
}

bool
Queen::verticalPossible(chess::Piece *(*board)[8], const int &fromX, const int &fromY, const int &toX, const int &toY) {
    return upPossible(board, fromX, fromY, toX, toY)
           || downPossible(board, fromX, fromY, toX, toY);
}

chess::Piece *Queen::getDeepCopy() {
    Queen* queen = new Queen(this->color);
    queen->moved = this->moved;

    return queen;
}

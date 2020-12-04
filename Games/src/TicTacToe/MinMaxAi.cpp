#include "TicTacToe/MinMaxAi.h"

ttt::MinMaxAi::MinMaxAi() {

}

ttt::MinMaxAi::~MinMaxAi() {

}

int ttt::MinMaxAi::getMove(std::string state, int color) {
    TicTacToeAdapter adap = TicTacToeAdapter();

    ttt::Board board = adap.convertStringToBoard(state);
    ttt::Move move = getMove(board, color);

    return move.x + move.y*3;
}

ttt::Move ttt::MinMaxAi::getMove(ttt::Board board, int color) {
    this->color = color;
    this->opponentColor = color%2+1;

    std::vector<ttt::Move> bestMoves;
    std::vector<ttt::Move> possibleMoves = getAllPossibleMoves(board);
    std::vector<int> values;

    for(int i = 0; i < possibleMoves.size(); i++) {
        ttt::Board copyBoard = ttt::Board(board);
        ttt::GameLogic::makeMove(copyBoard, color, possibleMoves[i]);
        values.push_back(evaluateBoard(copyBoard, opponentColor, false, INT_MIN, INT_MAX));
    }
    bestMoves = getBestMoves(possibleMoves, values);

    return getRandomMove(bestMoves);
}

int ttt::MinMaxAi::evaluateBoard(ttt::Board board, int currentPlayer, bool maximizingPlayer, int alpha, int beta) {
    int nextPlayer = ttt::GameLogic::getNextPlayer(currentPlayer);
    if(ttt::GameLogic::playerWon(board, color))
        return 1;
    else if(ttt::GameLogic::playerWon(board, opponentColor))
        return -1;
    else if(ttt::GameLogic::isBoardFull(board))
        return 0;

    int boardValue = 0;
    if(maximizingPlayer)
        boardValue = -10;
    else
        boardValue = 10;

    std::vector<ttt::Move> allPossibleMoves = getAllPossibleMoves(board);
    for(int i = 0; i < allPossibleMoves.size(); i++) {
        ttt::Board copyBoard = ttt::Board(board);
        ttt::Move currentMove = allPossibleMoves[i];
        ttt::GameLogic::makeMove(copyBoard,currentPlayer, currentMove);
        int moveValue = evaluateBoard(copyBoard, nextPlayer, !maximizingPlayer, alpha, beta);

        if(maximizingPlayer) {
            boardValue = std::max(moveValue, boardValue);
            alpha = std::max(boardValue, alpha);
            if (beta <= alpha)
                break;
        } else {
            boardValue = std::min(moveValue, boardValue);
            beta = std::min(boardValue, beta);
            if(beta <= alpha)
                break;
        }
    }

    return boardValue;
}

std::vector<ttt::Move> ttt::MinMaxAi::getAllPossibleMoves(const ttt::Board &board) {
    std::vector<ttt::Move> allPossibleMoves;

    for(int x = 0; x < 3; x++) {
        for(int y = 0; y < 3; y++) {
            if(board.board[x][y] == 0)
                allPossibleMoves.push_back(ttt::Move(x, y));
        }
    }

    return allPossibleMoves;
}

std::vector<ttt::Move> ttt::MinMaxAi::getBestMoves(std::vector<ttt::Move> &moves, std::vector<int> &values) {
    std::vector <ttt::Move> bestMoves;
    int highestValue = *(std::max_element(values.begin(), values.end()));

    for(int i = 0; i < moves.size(); i++) {
        if(values[i] == highestValue)
            bestMoves.push_back(moves[i]);
    }

    return bestMoves;
}

ttt::Move ttt::MinMaxAi::getRandomMove(const std::vector<ttt::Move> &moves) {
    int randIndex = rand() % moves.size();
    return moves[randIndex];
}
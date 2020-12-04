#ifndef PROTOTYPE_TICTACTOE_H
#define PROTOTYPE_TICTACTOE_H

#include <iostream>
#include <chrono>
#include <thread>
#include "GameLogic.h"
#include "AI/Ai.h"
#include "Move.h"
#include "Board.h"
#include "Renderer.h"
#include "SDL/Mouse.h"

class TicTacToe {

public:
    TicTacToe();
    TicTacToe(Ai* ai, int aiColor);
    ~TicTacToe();

    void updateGame();
    void reset();
    void gameLoop();
    static void printBoardToConsole(const ttt::Board &board);

private:
    void updateTwoPlayerGame();
    void updateOnePlayerGame();
    void updateAiMove(int &currentPlayer, ttt::Board &board);
    void updateHumanPlayerMove(int &currentPlayer, ttt::Board &board);
    void handleGameOver();
    inline bool isValid(int x, int y);
    bool isValidMove(const ttt::Move &move, const ttt::Board &board);
    static char convertPlayerNumberToChar(const int &number);
    std::string convertBoardToString(const ttt::Board &board);
    ttt::Move getMoveFromConsole();
    ttt::Move getMoveFromMouseInput();
    void outputBoard(const ttt::Board &board);
    ttt::Move getInput();
    void outputGameWinnerToConsole(const ttt::Board &board, const int currentPlayer);

    Ai* ai;

    int currentPlayer = 1;
    int playerCount;
    int aiColor;
    ttt::Board board;
    bool quit = false;
    bool graphicalOutput = true;
    ttt::Renderer* renderer;
    Mouse mouse = Mouse();
};


#endif //PROTOTYPE_TICTACTOE_H

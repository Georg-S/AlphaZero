#pragma once
#include <ConnectFour/Renderer.h>
#include <Chess/Renderer.h>
#include <Chess/Engine/BitBoard.h>
#include <TicTacToe/Renderer.h>

void renderBoard(const ttt::Board& board);
void renderBoard(const cn4::Board& board);
void renderBoard(const ceg::BitBoard& board);
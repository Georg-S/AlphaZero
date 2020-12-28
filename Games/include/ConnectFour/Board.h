#ifndef DEEPREINFORCEMENTLEARNING_cn4_BOARD_H
#define DEEPREINFORCEMENTLEARNING_cn4_BOARD_H

#include <iostream>
#include <string>
#include "PlayerColor.h"

namespace cn4 
{
	struct Board 
	{
		Board() 
		{
			for (int x = 0; x < width; x++)
				for (int y = 0; y < height; y++)
					board[x][y] = (int)cn4::PlayerColor::NONE;
		}

		Board(const cn4::Board& board) 
		{
			for (int x = 0; x < width; x++)
				for (int y = 0; y < height; y++) 
					this->board[x][y] = board.board[x][y];
		}

		char convertPlayerColorToChar(int color) const
		{
			if (color == (int)cn4::PlayerColor::NONE)
				return '-';
			return color == 1 ? '0' : '1';
		}

		void print() const
		{
			for (int y = height-1; y >= 0; y--) 
			{
				for (int x = 0; x < width; x++) 
				{
					std::cout << convertPlayerColorToChar(board[x][y]) << " ";
				}
				std::cout << std::endl;
			}
		}

		std::string toString() const
		{
			std::string result;

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					result += std::to_string(board[x][y]);
				}
			}

			return result;
		}

		const static int width = 7;
		const static int height = 6;
		int board[width][height];
	};
}

#endif //DEEPREINFORCEMENTLEARNING_cn4_BOARD_H

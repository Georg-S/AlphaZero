#pragma once
#include <iostream>

namespace ceg
{
	static void switch_x_and_y(int(&arr)[8][8])
	{
		for (int y = 0; y < 8; y++)
		{
			for (int x = y + 1; x < 8; x++)
			{
				std::swap(arr[x][y], arr[y][x]);
			}
		}
	}

	static void flip_x(int* arr)
	{
		for (int y = 0; y < 8; y++)
		{
			for (int left = 0, right = 8 - 1; left < right; left++, right--)
			{
				int index_1 = y * 8 + left;
				int index_2 = y * 8 + right;

				if (arr[index_1] != arr[index_2])
					std::swap(arr[index_1], arr[index_2]);
			}
		}
	}

	static void flip_x(int(&arr)[8][8])
	{
		for (int y = 0; y < 8; y++)
		{
			for (int left = 0, right = 8 - 1; left < right; left++, right--)
			{
				if (arr[left][y] != arr[right][y])
					std::swap(arr[left][y], arr[right][y]);
			}
		}
	}

	static void flip_y(int(&arr)[8][8])
	{
		for (int x = 0; x < 8; x++)
		{
			for (int top = 0, bottom = 8 - 1; top < bottom; top++, bottom--)
			{

				if (arr[x][top] != arr[x][bottom])
					std::swap(arr[x][top], arr[x][bottom]);
			}
		}
	}

	static void flip_y(int* arr)
	{
		for (int x = 0; x < 8; x++)
		{
			for (int top = 0, bottom = 8 - 1; top < bottom; top++, bottom--)
			{
				int index_1 = x + top * 8;
				int index_2 = x + bottom * 8;

				if (arr[index_1] != arr[index_2])
					std::swap(arr[index_1], arr[index_2]);
			}
		}
	}

	static void copy(int(&destination)[8][8], const int(*source)[8])
	{
		for (int x = 0; x < 8; x++)
		{
			for (int y = 0; y < 8; y++)
			{
				destination[x][y] = source[x][y];
			}
		}
	}

	static void copy(int* destination, const int* source)
	{
		for (int x = 0; x < 8; x++)
		{
			for (int y = 0; y < 8; y++)
			{
				int index = x + y * 8;

				destination[index] = source[index];
			}
		}
	}


	static bool is_equal(const int(*arr1)[8], const int(*arr2)[8])
	{
		for (int y = 0; y < 8; y++)
		{
			for (int x = 0; x < 8; x++)
			{
				if (arr1[x][y] != arr2[x][y])
					return false;
			}
		}
		return true;
	}

	static bool is_equal(const int* arr1, const int* arr2)
	{
		for (int y = 0; y < 8; y++)
		{
			for (int x = 0; x < 8; x++)
			{
				int index = x + y * 8;

				if (arr1[index] != arr2[index])
					return false;
			}
		}
		return true;
	}

	static void print_values(const int(*arr)[8])
	{
		for (int y = 0; y < 8; y++)
		{
			for (int x = 0; x < 8; x++)
			{
				std::cout << arr[x][y] << ", ";
			}
			std::cout << std::endl;
		}
	}

	static void print_values(const int* arr)
	{
		for (int y = 0; y < 8; y++)
		{
			for (int x = 0; x < 8; x++)
			{
				int index = x + y * 8;

				std::cout << arr[index] << ", ";
			}
			std::cout << std::endl;
		}
	}

	// Most Valuable Victim, Least Valuable Attacker
	constexpr int MVV_LVA[7][7]
	{
		{0, 0, 0, 0, 0, 0, 0} ,       // Victim: King, Attacker: King, Queen, Rook, Bishop, Knight, Pawn, None
		{50, 51, 52, 53, 54, 55, 0}, //  Victim: Queen, King, Attacker: King, Queen, Rook, Bishop, Knight, Pawn, None
		{40, 41, 42, 43, 44, 45, 0}, //  Victim: Rook, King, Attacker: King, Queen, Rook, Bishop, Knight, Pawn, None
		{30, 31, 32, 33, 34, 35, 0}, //  Victim: Bishop, King, Attacker: King, Queen, Rook, Bishop, Knight, Pawn, None
		{20, 21, 22, 23, 24, 25, 0}, //  Victim: Knight, King, Attacker: King, Queen, Rook, Bishop, Knight, Pawn, None
		{10, 11, 12, 13, 14, 15, 0 }, //  Victim: Pawn, King, Attacker: King, Queen, Rook, Bishop, Knight, Pawn, None
		{0, 0, 0, 0, 0, 0, 0},       //  Victim: None, King, Attacker: King, Queen, Rook, Bishop, Knight, Pawn, None
	};

	constexpr int white_pawn_table[64] =
	{
	 0,  0,  0,  0,  0,  0,  0,  0,
	50, 50, 50, 50, 50, 50, 50, 50,
	10, 10, 20, 30, 30, 20, 10, 10,
	 5,  5, 10, 25, 25, 10,  5,  5,
	 0,  0,  0, 20, 20,  0,  0,  0,
	 5, -5,-10,  0,  0,-10, -5,  5,
	 5, 10, 10,-20,-20, 10, 10,  5,
	 0,  0,  0,  0,  0,  0,  0,  0
	};

	constexpr int black_pawn_table[64] =
	{
	0, 0, 0, 0, 0, 0, 0, 0,
	5, 10, 10, -20, -20, 10, 10, 5,
	5, -5, -10, 0, 0, -10, -5, 5,
	0, 0, 0, 20, 20, 0, 0, 0,
	5, 5, 10, 25, 25, 10, 5, 5,
	10, 10, 20, 30, 30, 20, 10, 10,
	50, 50, 50, 50, 50, 50, 50, 50,
	0, 0, 0, 0, 0, 0, 0, 0,
	};

	constexpr int white_knight_table[64] =
	{
	-50,-40,-30,-30,-30,-30,-40,-50,
	-40,-20,  0,  0,  0,  0,-20,-40,
	-30,  0, 10, 15, 15, 10,  0,-30,
	-30,  5, 15, 20, 20, 15,  5,-30,
	-30,  0, 15, 20, 20, 15,  0,-30,
	-30,  5, 10, 15, 15, 10,  5,-30,
	-40,-20,  0,  5,  5,  0,-20,-40,
	-50,-40,-30,-30,-30,-30,-40,-50,
	};

	constexpr int black_knight_table[64] =
	{
	-50, -40, -30, -30, -30, -30, -40, -50,
	-40, -20, 0, 5, 5, 0, -20, -40,
	-30, 5, 10, 15, 15, 10, 5, -30,
	-30, 0, 15, 20, 20, 15, 0, -30,
	-30, 5, 15, 20, 20, 15, 5, -30,
	-30, 0, 10, 15, 15, 10, 0, -30,
	-40, -20, 0, 0, 0, 0, -20, -40,
	-50, -40, -30, -30, -30, -30, -40, -50,
	};

	constexpr int white_bishop_table[64] =
	{
	-20,-10,-10,-10,-10,-10,-10,-20,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-10,  0,  5, 10, 10,  5,  0,-10,
	-10,  5,  5, 10, 10,  5,  5,-10,
	-10,  0, 10, 10, 10, 10,  0,-10,
	-10, 10, 10, 10, 10, 10, 10,-10,
	-10,  5,  0,  0,  0,  0,  5,-10,
	-20,-10,-10,-10,-10,-10,-10,-20,
	};

	constexpr int black_bishop_table[64] =
	{
	-20, -10, -10, -10, -10, -10, -10, -20,
	-10, 5, 0, 0, 0, 0, 5, -10,
	-10, 10, 10, 10, 10, 10, 10, -10,
	-10, 0, 10, 10, 10, 10, 0, -10,
	-10, 5, 5, 10, 10, 5, 5, -10,
	-10, 0, 5, 10, 10, 5, 0, -10,
	-10, 0, 0, 0, 0, 0, 0, -10,
	-20, -10, -10, -10, -10, -10, -10, -20,
	};

	constexpr int white_rook_table[64] =
	{
	  0,  0,  0,  0,  0,  0,  0,  0,
	  5, 10, 10, 10, 10, 10, 10,  5,
	 -5,  0,  0,  0,  0,  0,  0, -5,
	 -5,  0,  0,  0,  0,  0,  0, -5,
	 -5,  0,  0,  0,  0,  0,  0, -5,
	 -5,  0,  0,  0,  0,  0,  0, -5,
	 -5,  0,  0,  0,  0,  0,  0, -5,
	  0,  0,  0,  5,  5,  0,  0,  0
	};

	constexpr int black_rook_table[64] =
	{
	0, 0, 0, 5, 5, 0, 0, 0,
	-5, 0, 0, 0, 0, 0, 0, -5,
	-5, 0, 0, 0, 0, 0, 0, -5,
	-5, 0, 0, 0, 0, 0, 0, -5,
	-5, 0, 0, 0, 0, 0, 0, -5,
	-5, 0, 0, 0, 0, 0, 0, -5,
	5, 10, 10, 10, 10, 10, 10, 5,
	0, 0, 0, 0, 0, 0, 0, 0,
	};

	constexpr int white_queen_table[64] =
	{
	-20,-10,-10, -5, -5,-10,-10,-20,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-10,  0,  5,  5,  5,  5,  0,-10,
	 -5,  0,  5,  5,  5,  5,  0, -5,
	  0,  0,  5,  5,  5,  5,  0, -5,
	-10,  5,  5,  5,  5,  5,  0,-10,
	-10,  0,  5,  0,  0,  0,  0,-10,
	-20,-10,-10, -5, -5,-10,-10,-20
	};

	constexpr int black_queen_table[64] =
	{
	-20, -10, -10, -5, -5, -10, -10, -20,
	-10, 0, 5, 0, 0, 0, 0, -10,
	-10, 5, 5, 5, 5, 5, 0, -10,
	0, 0, 5, 5, 5, 5, 0, -5,
	-5, 0, 5, 5, 5, 5, 0, -5,
	-10, 0, 5, 5, 5, 5, 0, -10,
	-10, 0, 0, 0, 0, 0, 0, -10,
	-20, -10, -10, -5, -5, -10, -10, -20,
	};

	constexpr int white_king_early_game_table[64] =
	{
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-20,-30,-30,-40,-40,-30,-30,-20,
	-10,-20,-20,-20,-20,-20,-20,-10,
	 20, 20,  0,  0,  0,  0, 20, 20,
	 20, 30, 10,  0,  0, 10, 30, 20
	};

	constexpr int black_king_early_game_table[64] =
	{
	20, 30, 10, 0, 0, 10, 30, 20,
	20, 20, 0, 0, 0, 0, 20, 20,
	-10, -20, -20, -20, -20, -20, -20, -10,
	-20, -30, -30, -40, -40, -30, -30, -20,
	-30, -40, -40, -50, -50, -40, -40, -30,
	-30, -40, -40, -50, -50, -40, -40, -30,
	-30, -40, -40, -50, -50, -40, -40, -30,
	-30, -40, -40, -50, -50, -40, -40, -30,
	};

	constexpr int white_king_end_game_table[64] =
	{
	-50,-40,-30,-20,-20,-30,-40,-50,
	-30,-20,-10,  0,  0,-10,-20,-30,
	-30,-10, 20, 30, 30, 20,-10,-30,
	-30,-10, 30, 40, 40, 30,-10,-30,
	-30,-10, 30, 40, 40, 30,-10,-30,
	-30,-10, 20, 30, 30, 20,-10,-30,
	-30,-30,  0,  0,  0,  0,-30,-30,
	-50,-30,-30,-30,-30,-30,-30,-50
	};

	constexpr int black_king_end_game_table[64] =
	{
	-50, -30, -30, -30, -30, -30, -30, -50,
	-30, -30, 0, 0, 0, 0, -30, -30,
	-30, -10, 20, 30, 30, 20, -10, -30,
	-30, -10, 30, 40, 40, 30, -10, -30,
	-30, -10, 30, 40, 40, 30, -10, -30,
	-30, -10, 20, 30, 30, 20, -10, -30,
	-30, -20, -10, 0, 0, -10, -20, -30,
	-50, -40, -30, -20, -20, -30, -40, -50,
	};

}
#include "Chess/Engine/Utility.h"

static constexpr int index64[64] = {
   63,  0, 58,  1, 59, 47, 53,  2,
   60, 39, 48, 27, 54, 33, 42,  3,
   61, 51, 37, 40, 49, 18, 28, 20,
   55, 30, 34, 11, 43, 14, 22,  4,
   62, 57, 46, 52, 38, 26, 32, 41,
   50, 36, 17, 19, 29, 10, 13, 21,
   56, 45, 25, 31, 35, 16,  9, 12,
   44, 24, 15,  8, 23,  7,  6,  5
};

// get_bit_index_lsb (bitScanForward)
// @author Charles E. Leiserson, Harald Prokop, Keith H. Randall
int ceg::get_bit_index_lsb(uint64_t num)
{
	constexpr uint64_t debruijn64 = uint64_t(0x07EDD5E59A4E28C2);
	return index64[((num & -num) * debruijn64) >> 58];
}

long long ceg::get_current_time_in_ms()
{
	auto current_time = std::chrono::system_clock::now();
	return std::chrono::time_point_cast<std::chrono::milliseconds>(current_time).time_since_epoch().count();
}

uint64_t ceg::get_lsb(uint64_t num)
{
	return num & -num;
}

void ceg::reset_lsb(uint64_t& num)
{
	num &= (num - 1);
}

bool ceg::is_number(char c)
{
	return (c >= '0') && (c <= '9');
}

void ceg::set_bit(uint64_t& num, int bit_index)
{
	num |= (uint64_t(1) << bit_index);
}

void ceg::set_bit(uint64_t& num, int x, int y)
{
	constexpr int board_width = 8;
	set_bit(num, x + board_width * y);
}

void ceg::clear_bit(uint64_t& num, int bit_index)
{
	num &= ~(uint64_t(1) << bit_index);
}

void ceg::clear_bit(uint64_t& num, int x, int y)
{
	constexpr int board_width = 8;
	clear_bit(num, x + board_width * y);
}

std::vector<std::string> ceg::string_split(std::string str, const std::string& delimiter)
{
	std::vector<std::string> result;
	size_t pos = 0;
	while ((pos = str.find(delimiter)) != std::string::npos)
	{
		result.push_back(str.substr(0, pos));
		str.erase(0, pos + delimiter.length());
	}
	result.push_back(str);

	return result;
}

bool ceg::is_bit_set(uint64_t num, int bit_index)
{
	return num & (uint64_t(1) << bit_index);
}

bool ceg::is_bit_set(uint64_t num, int x, int y)
{
	return is_bit_set(num, x + y * 8);
}

int ceg::to_linear_idx(int x, int y)
{
	return x + y * 8;
}

void ceg::print_bit_board(uint64_t num)
{
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			char c = is_bit_set(num, to_linear_idx(x, y)) ? '1' : '-';
			std::cout << c << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

uint64_t ceg::set_all_bits_in_direction(int pos_x, int pos_y, int dir_x, int dir_y, bool set_pos)
{
	uint64_t result = 0;
	while (in_board_bounds(pos_x) && in_board_bounds(pos_y))
	{
		if (set_pos)
			set_bit(result, pos_x, pos_y);
		pos_x += dir_x;
		pos_y += dir_y;
		set_pos = true;
	}

	return result;
}

uint64_t ceg::set_all_bits_in_direction_until_occupied(int index, int dir_x, int dir_y, uint64_t occupied)
{
	uint64_t result = 0;

	int x = index % 8 + dir_x;
	int y = index / 8 + dir_y;
	while (in_board_bounds(x) && in_board_bounds(y)) 
	{
		set_bit(result, x, y);
		if (is_bit_set(occupied, x, y))
			break;

		x += dir_x;
		y += dir_y;
	}

	return result;
}

bool ceg::in_board_bounds(int index)
{
	return index >= 0 && index < 8; ;
}

static uint64_t create_number(const std::vector<int>& bit_indices, const std::vector<int>& values) 
{
	uint64_t result = 0;
	for (int i = 0; i < bit_indices.size(); i++) 
	{
		if (values[i] == 1)
			ceg::set_bit(result, bit_indices[i]);
	}

	return result;
}

std::vector<uint64_t> ceg::get_every_bit_combination(const std::vector<int>& bit_indices)
{
	auto result = std::vector<uint64_t>();
	result.push_back(0);

	if(bit_indices.size() == 0)
		return result;

	std::vector<int> values = std::vector<int>(bit_indices.size());
	bool not_finished = true;
	int index = 0;

	while (not_finished)
	{
		if (index >= values.size()) 
		{
			not_finished = true;
			break;
		}

		if (values[index] == 0) 
		{
			values[index] = 1;
			index = 0;
			result.push_back(create_number(bit_indices, values));
		}
		else if (values[index] == 1) 
		{
			values[index] = 0;
			index++;
		}
	}

	return result;
}

std::vector<int> ceg::get_bit_indices(uint64_t num)
{
	std::vector<int> result = std::vector<int>();

	while (num != 0) 
	{
		result.push_back(ceg::get_bit_index_lsb(num));
		ceg::reset_lsb(num);
	}
	return result;
}

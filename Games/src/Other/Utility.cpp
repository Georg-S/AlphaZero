#include "Other/Utility.h"

using namespace game;

int game::lowestBitIndex(uint32_t num)
{
	static constexpr uint8_t BitPositionLookup[32] = // hash table
	{
	  0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
	  31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
	};
	constexpr uint32_t debruijn32 = uint32_t(0x077CB531U);
	return BitPositionLookup[(((num & -num) * debruijn32)) >> 27];
}

int game::lowestBitIndex(uint64_t num)
{
	static constexpr int index64[64] =
	{
	   63,  0, 58,  1, 59, 47, 53,  2,
	   60, 39, 48, 27, 54, 33, 42,  3,
	   61, 51, 37, 40, 49, 18, 28, 20,
	   55, 30, 34, 11, 43, 14, 22,  4,
	   62, 57, 46, 52, 38, 26, 32, 41,
	   50, 36, 17, 19, 29, 10, 13, 21,
	   56, 45, 25, 31, 35, 16,  9, 12,
	   44, 24, 15,  8, 23,  7,  6,  5
	};

	constexpr uint64_t debruijn64 = uint64_t(0x07EDD5E59A4E28C2);
	return index64[((num & -num) * debruijn64) >> 58];
}

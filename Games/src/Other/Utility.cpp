#include "Other/Utility.h"

using namespace game;

void game::setBit(uint32_t& num, int bitIndex)
{
	num |= uint32_t(1) << bitIndex;
}

void game::resetLowestBit(uint32_t& num)
{
    num &= (num - 1);
}

int game::lowestBitIndex(uint32_t num)
{
    static constexpr uint8_t BitPositionLookup[32] = // hash table
    {
      0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
      31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
    };
    return BitPositionLookup[(((num & -num) * 0x077CB531U)) >> 27];
}

bool game::isBitSet(uint32_t num, int bitIndex)
{
    return num & (uint32_t(1) << bitIndex);
}

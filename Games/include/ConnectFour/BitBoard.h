#ifndef DEEPREINFORCEMENTLEARNING_cn4_BITBOARD_H
#define DEEPREINFORCEMENTLEARNING_cn4_BITBOARD_H

#include <cstdint>
#include <ConnectFour/Board.h>
#include <map>
#include <bitset>

namespace cn4
{
	class BitBoard
	{
	public:
		BitBoard();
		BitBoard(const cn4::Board& inputBoard, int currentPlayer);
		uint64_t convertBoardIntoBitBoard(const cn4::Board& inputBoard);
		void writeBinaryValueToBoard(uint64_t value, int bitPos);
		void writeSingleBitToPositionOnBoard(int bitPosition, unsigned int bitValue);
		void writeSingleBitToPosition(uint64_t& inp, int bitPosition, unsigned int bitValue);
		void makeMove(int col);
		bool movePossible(int col);
		void print();
		void print(uint64_t board);
		uint64_t convertIntToBit(int value);
		uint64_t getCurrentBitRepresentation();

	private:
		// Encodes current board into:
		// First 42 bits are 1 for current player and 0 for last player 
		// Then 21 bits that mark the size of each column  7 * 3bits
		uint64_t currentPosition;

		static const int width = 7;
		static const int height = 6;
	};
}


#endif	//DEEPREINFORCEMENTLEARNING_cn4_BITBOARD_H
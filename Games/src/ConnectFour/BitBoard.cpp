#include "ConnectFour/BitBoard.h"

cn4::BitBoard::BitBoard() : currentPosition(0)
{

}

cn4::BitBoard::BitBoard(const cn4::Board& inputBoard, int currentPlayer)
{
	currentPosition = convertBoardIntoBitBoard(inputBoard);
}

uint64_t cn4::BitBoard::convertBoardIntoBitBoard(const cn4::Board& inputBoard)
{
	currentPosition = 0;

	for (int y = 0; y < inputBoard.height; y++)
	{
		for (int x = 0; x < inputBoard.width; x++)
		{
			uint64_t pos = x + y * width;
			int bitVal = convertIntToBit(inputBoard.board[x][y]);
			writeSingleBitToPositionOnBoard(pos, bitVal);
		}
	}

	int bitPos = 42;

	for (int x = 0; x < width; x++)
	{
		int size = 0;
		for (int y = 0; y < height; y++)
		{
			if (inputBoard.board[x][y] == 0)
				break;
			size++;
		}
		uint64_t columnSize = size;
		writeBinaryValueToBoard(columnSize, bitPos);
		bitPos += 3;
	}
}

void cn4::BitBoard::writeBinaryValueToBoard(uint64_t value, int bitPos)
{
	//Writes the value to the specified position (only works if the values are 0)

	value = value << bitPos;
	currentPosition |= value;
}

void cn4::BitBoard::writeSingleBitToPositionOnBoard(int bitPosition, unsigned int bitValue)
{
	writeSingleBitToPosition(this->currentPosition, bitPosition, bitValue);
}

void cn4::BitBoard::writeSingleBitToPosition(uint64_t& inp, int bitPosition, unsigned int bitValue)
{
	if (bitValue == 1)
		inp |= (uint64_t)1 << bitPosition;
	else if (bitValue == 0)
	{
		uint64_t bit = (uint64_t)1 << bitPosition;
		bit = UINT64_MAX^bit;
		inp &= bit;
	}
}

void cn4::BitBoard::makeMove(int col)
{

}

bool cn4::BitBoard::movePossible(int col)
{
	return false;
}

void cn4::BitBoard::print()
{
	print(currentPosition);
}

void cn4::BitBoard::print(uint64_t board)
{
	for (int y = height-1; y >= 0; y--)
	{
		for (int x = 0; x < width; x++)
		{
			uint64_t pos = x + y * width;
			uint64_t bit = (uint64_t)1 << pos;

			uint64_t result = board & bit;

			if (result == 0)
				std::cout << 0 << " ";
			else
				std::cout << 1 << " ";

		}
		std::cout << std::endl;
	}
}

uint64_t cn4::BitBoard::convertIntToBit(int value)
{
	if (value == 1)
		return 1;
	if (value == 2)
		return 0;
	return 0;
}

uint64_t cn4::BitBoard::getCurrentBitRepresentation()
{
	return currentPosition;
}


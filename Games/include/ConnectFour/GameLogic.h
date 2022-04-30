#ifndef DEEPREINFORCEMENTLEARNING_cn4_GAMELOGIC_H
#define DEEPREINFORCEMENTLEARNING_cn4_GAMELOGIC_H

#include <cstdint>
#include <string>
#include "Other/Utility.h"

namespace cn4
{
	constexpr int boardWidth = 7;
	constexpr int boardHeight = 6;
	constexpr uint64_t rightTwoColumnsZeroMask = uint64_t(1073538912159);
	constexpr uint64_t rightColumnZeroMask = uint64_t(2181708111807);
	constexpr uint64_t leftColumnZeroMask = uint64_t(4363416223614);
	constexpr uint64_t leftTwoColumnsZeroMask = uint64_t(4294155648636);
	constexpr uint64_t fullBoardMask = uint64_t(4398046511103);
	enum class PlayerColor { NONE, YELLOW, RED };

	class Board
	{
	public:
		Board() = default;
		Board(const std::string& str);
		bool isFull() const;
		uint64_t occupied() const;
		uint64_t yellowPieces() const;
		uint64_t redPieces() const;
		void makeMove(int column, PlayerColor color);
		PlayerColor at(int column, int row) const;
		std::string toString() const;

	private:
		uint64_t m_yellowPieces = 0;
		uint64_t m_redPieces = 0;
		uint64_t m_occupied = 0;
		int m_occupiedColumnSize[boardWidth] = {};
	};
	
	PlayerColor getPlayerWon(const Board& board);
	bool isGameOver(const Board& board);
	PlayerColor getNextPlayer(PlayerColor color);
	int getNextPlayer(int playerColor);
	bool hasPlayerWon(uint64_t playerPieces);
	bool verticalFourInARow(uint64_t pieces);
	bool horizontalFourInARow(uint64_t pieces);
	bool diagonalFourInARow(uint64_t pieces);
	bool diagonalDownFourInARow(uint64_t pieces);
	bool diagonalUpFourInARow(uint64_t pieces);
	bool isMovePossible(const Board& board, int move);
	std::vector<int> getAllPossibleMoves(const Board& board);
}

#endif //DEEPREINFORCEMENTLEARNING_cn4_GAMELOGIC_H

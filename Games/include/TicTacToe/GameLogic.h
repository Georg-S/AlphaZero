#ifndef DEEPREINFORCEMENTLEARNING_ttt_GAMELOGIC_H
#define DEEPREINFORCEMENTLEARNING_ttt_GAMELOGIC_H

#include <cassert>
#include <cstdint>
#include <vector>
#include <string>
#include "Other/Utility.h"

namespace ttt
{
	struct Move
	{
		Move() = default;
		Move(int x, int y) : x(x), y(y) {};
		Move(int rawMove)
		{
			x = rawMove % 3;
			y = rawMove / 3;
		}
		int x;
		int y;
	};

	enum class PlayerColor { NONE = 0, CROSS, DOT };

	class Board
	{
	public:
		Board();
		Board(const std::string& str);
		void makeMove(int pos, PlayerColor color);
		void makeMove(const Move& move, PlayerColor color);
		bool isBoardFull() const;
		bool isMovePossible(const Move& move) const;
		PlayerColor at(int x, int y) const;
		uint32_t getPieces(PlayerColor color) const;
		uint32_t occupied() const;
		std::string toString() const;
		friend bool operator<(const Board& lhs, const Board& rhs);

		static constexpr uint32_t fullBoard = 511;
	private:
		uint32_t m_crosses;
		uint32_t m_dots;
		uint32_t m_occupied;
	};

	bool isGameOver(const Board& board);
	bool playerWon(const Board& board, PlayerColor color);
	bool horizontalThreeInARow(uint32_t pieces);
	bool verticalThreeInARow(uint32_t pieces);
	bool diagonalThreeInARow(uint32_t pieces);
	PlayerColor getNextPlayer(PlayerColor color);
	int getNextPlayer(int player);
	template<typename T>
	std::vector<T> getAllPossibleMoves(const Board& board)
	{
		std::vector<T> moves;

		auto remainingPieces = ~board.occupied();
		remainingPieces &= Board::fullBoard;

		while (remainingPieces)
		{
			int move = game::lowestBitIndex(remainingPieces);
			moves.emplace_back(move);
			game::resetLowestBit(remainingPieces);
		}

		return moves;
	}
}
#endif //DEEPREINFORCEMENTLEARNING_ttt_GAMELOGIC_H

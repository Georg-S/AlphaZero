#pragma once
#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
#include "Utility.h"

namespace ceg
{
	constexpr int board_width = 8;
	constexpr int board_height = 8;

	struct Pieces
	{
		uint64_t pawns = 0;
		uint64_t bishops = 0;
		uint64_t knights = 0;
		uint64_t rooks = 0;
		uint64_t queens = 0;
		uint64_t king = 0;
		uint64_t occupied = 0;
		uint64_t castling = 0;
	};

	struct InternalMove
	{
		int from;
		int to;
		int value;
	};

	inline bool operator<(const ceg::InternalMove& rhs, const ceg::InternalMove& lhs)
	{
		return rhs.value < lhs.value;
	}

	inline bool operator>(const ceg::InternalMove& rhs, const ceg::InternalMove& lhs)
	{
		return rhs.value > lhs.value;
	}

	class BitBoard
	{
	public:
		BitBoard() = default;
		BitBoard(const std::string& FEN_str);
		BitBoard(const std::string& FEN_pieces_str, const std::string& FEN_castling_str, const std::string& FEN_en_passant_str = "");

		void move_piece(Pieces* pieces, const InternalMove& move);
		void clear_bit_for_pieces(Pieces* pieces, int bit_index);
		void clear_bits_at_position(int bit_index);
		uint64_t* get_ptr_to_piece(Pieces* pieces, int bit_index) const;
		void print_board_to_console() const;
		void set_board(const std::string& FEN_pieces_str, const std::string& FEN_castling_str = "", const std::string& FEN_en_passant_str = "");
		void update_occupied();
		std::vector<std::vector<char>> get_fen_char_representation() const;
		std::string getPiecesFENString() const;
		std::string getCastlingFENString() const;
		std::string getEnPassantFENString() const;

		Pieces white_pieces;
		Pieces black_pieces;

		uint64_t en_passant_mask = 0;
		uint64_t occupied = 0;
	private:
		void set_en_passant(const std::string& FEN_str);
		void set_castling(const std::string& FEN_castling_str);
		bool is_occupied(int x, int y) const;
		char get_field_char(int x, int y) const;
		char get_FEN_char(int x, int y) const;
		void set_piece_by_FEN_char(char c, int x, int y);
	};

	std::string to_FEN_string(const BitBoard& board, bool currentPlayerBlack);
}
#pragma once
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "BitBoard.h"
#include "Utility.h"

namespace ceg
{
	struct CheckInfo
	{
		CheckInfo()
		{
			std::fill_n(pin_mask, 64, ~uint64_t(0));
		}

		int check_counter = 0;
		uint64_t attacked_fields = 0;
		uint64_t check_piece = ~uint64_t(0); // Check piece mask is needed for an en passant edge case
		uint64_t pin_mask[64];
		uint64_t check_mask_with_piece = ~uint64_t(0);
	};

	struct StateInformation
	{
		StateInformation(int counter, std::vector<ceg::InternalMove>&& moves)
			: check_counter(counter)
			, possible_moves(std::move(moves))
		{}

		int check_counter;
		std::vector<ceg::InternalMove> possible_moves;
	};

	class MoveGenerator
	{
	public:
		MoveGenerator();
		CheckInfo get_check_info(BitBoard board, bool black) const;
		std::vector<InternalMove> get_all_possible_moves(const BitBoard& board, bool black) const;
		StateInformation get_state_information(const BitBoard& board, bool black) const;

		void make_move(BitBoard& board, const InternalMove& move) const;
		void make_move(BitBoard& board, const InternalMove& move, bool black) const;
		void make_move_with_auto_promotion(BitBoard& board, const InternalMove& move) const;
	private:
		uint64_t get_rook_moves(int index, uint64_t occupied) const;
		uint64_t get_bishop_moves(int index, uint64_t occupied) const;
		uint64_t get_queen_moves(int index, uint64_t occupied) const;
		uint64_t get_vertical_moves(int index, uint64_t occupied) const;
		uint64_t get_horizontal_moves(int index, uint64_t occupied) const;
		uint64_t get_diagonal_up_moves(int index, uint64_t occupied) const;
		uint64_t get_diagonal_down_moves(int index, uint64_t occupied) const;

		ceg::CheckInfo get_check_info(Pieces* player, const Pieces* other, const BitBoard& board, const uint64_t* pawn_attack_moves) const;
		StateInformation get_state_information(Pieces* playing, ceg::Pieces* other, const BitBoard& board,
			const uint64_t* pawn_normal_moves, const uint64_t* pawn_attack_moves, bool black) const;
		std::vector<InternalMove> get_all_possible_moves(Pieces* playing, ceg::Pieces* other, const BitBoard& board,
			const uint64_t* pawn_normal_moves, const uint64_t* pawn_attack_moves, bool black) const;
		std::vector<InternalMove> get_all_possible_moves(Pieces* playing, ceg::Pieces* other, const BitBoard& board,
			const uint64_t* pawn_normal_moves, const uint64_t* pawn_attack_moves, bool black, const CheckInfo& info) const;

		void init();
		void init_pawn_end_of_board_mask();
		void init_en_passant_capture_mask();
		void init_reset_index_mask();
		void init_castling_mask();
		void combine_two_masks(uint64_t* dest, uint64_t* source_1, uint64_t* source_2, int size = arr_size) const;
		void init_mask(uint64_t* mask, int x_dir, int y_dir, bool set_inital_index) const;
		void init_mask_with_occupied(std::unordered_map<uint64_t, uint64_t>* arr, uint64_t* mask, int x_dir, int y_dir) const;
		void init_knight_moves();
		void init_king_moves();
		void init_pawn_moves();

		static constexpr int arr_size = board_height * board_width;
		static constexpr int black_queen_tower_idx = 0;
		static constexpr int black_king_tower_idx = 7;
		static constexpr int white_queen_tower_idx = 56;
		static constexpr int white_king_tower_idx = 63;
		static constexpr int black_queen_castling_move_idx = 2;
		static constexpr int black_king_castling_move_idx = 6;	
		static constexpr int white_queen_castling_move_idx = 58;
		static constexpr int white_king_castling_move_idx = 62;
		uint64_t pawn_end_of_board_mask = 0;
		uint64_t pawn_end_of_board_inverted_mask = 0;
		uint64_t black_queen_side_castling_mask = 0;
		uint64_t black_king_side_castling_mask = 0;
		uint64_t white_queen_side_castling_mask = 0;
		uint64_t white_king_side_castling_mask = 0;
		uint64_t black_queen_side_castling_occupied_mask = 0;
		uint64_t white_queen_side_castling_occupied_mask = 0;
		uint64_t en_passant_capture_mask = 0;
		uint64_t vertical_mask[arr_size]{};
		uint64_t vertical_mask_without_index[arr_size]{};
		uint64_t horizontal_mask[arr_size]{};
		uint64_t horizontal_mask_without_index[arr_size]{};
		uint64_t diagonal_up_mask[arr_size]{};
		uint64_t diagonal_up_mask_without_index[arr_size]{};
		uint64_t diagonal_down_mask[arr_size]{};
		uint64_t diagonal_down_mask_without_index[arr_size]{};
		uint64_t reset_index_mask[arr_size]{};
		uint64_t knight_moves[arr_size]{};
		uint64_t king_moves[arr_size]{};
		uint64_t black_pawn_normal_moves[arr_size]{};
		uint64_t black_pawn_attack_moves[arr_size]{};
		uint64_t white_pawn_normal_moves[arr_size]{};
		uint64_t white_pawn_attack_moves[arr_size]{};
		std::unordered_map<uint64_t, uint64_t> horizontal_with_occupied[arr_size]{};
		std::unordered_map<uint64_t, uint64_t> vertical_with_occupied[arr_size]{};
		std::unordered_map<uint64_t, uint64_t> diagonal_up_with_occupied[arr_size]{};
		std::unordered_map<uint64_t, uint64_t> diagonal_down_with_occupied[arr_size]{};
	};
}
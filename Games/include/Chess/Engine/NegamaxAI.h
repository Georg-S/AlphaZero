#pragma once
#include <time.h>
#include <thread>
#include <mutex>
#include <iostream>
#include <random>
#include <AlphaZeroUtility.h>
#include "ValueTables.h"
#include "BitBoard.h"
#include "MoveGenerator.h"

namespace ceg
{
	struct TTEntry
	{
		std::mutex mut;
		enum class type { EXACT, UPPER, LOWER } type;
		uint64_t hash = 0;
		int value = 0;
		int depth = 0;
		ceg::InternalMove best_move;
	};

	class NegamaxAI
	{
	public:
		NegamaxAI(MoveGenerator* move_generator);
		NegamaxAI(const NegamaxAI&) = delete;
		NegamaxAI& operator=(const NegamaxAI&) = delete;
		~NegamaxAI();
		ceg::InternalMove get_move(const ceg::BitBoard& board, bool color_is_black, int depth = 5);
		ceg::InternalMove get_move(const ceg::BitBoard& board, bool color_is_black, int min_depth, int max_depth, long long time_in_ms);

	private:
		ceg::InternalMove iterative_deepening(const ceg::BitBoard& board, bool color_is_black, int in_min_depth, int max_depth, long long max_time_in_ms);
		void init_hashing_table();
		uint64_t hash_board(const ceg::BitBoard& board, bool color_is_black) const;
		std::vector<ceg::InternalMove> get_evaluated_moves(const ceg::BitBoard& board, bool color_is_black, int depth);
		std::vector<ceg::InternalMove> get_evaluated_moves(const ceg::BitBoard& board, bool color_is_black, int depth, const std::vector<ceg::InternalMove>& possible_moves);
		std::vector<ceg::InternalMove> get_evaluated_moves_multi_threaded(const ceg::BitBoard& board, bool color_is_black, int depth);
		void eval_multi_threaded(const ceg::BitBoard& board, bool color_is_black, const std::vector<ceg::InternalMove>& possible_moves, int depth);
		ceg::InternalMove get_random_move(const std::vector<ceg::InternalMove>& moves) const;
		void sort_possible_moves(const ceg::BitBoard& board, std::vector<ceg::InternalMove>& moves, bool color_is_black, const ceg::InternalMove& tt_move) const;
		int evaluate_board_negamax(const ceg::BitBoard& board, bool color_is_black, int depth, int alpha, int beta);
		int static_board_evaluation(const ceg::BitBoard& board, bool current_player_black) const;
		int get_pieces_value(const ceg::BitBoard& board, ceg::Pieces pieces, bool black_pieces) const;
		void hash_piece(uint64_t& current_hash, uint64_t piece, int table_index) const;
		std::vector<ceg::InternalMove> get_best_moves(const std::vector<ceg::InternalMove>& moves) const;
		int get_piece_count(uint64_t piece) const;
		bool is_end_game(const BitBoard& board) const;

		std::vector<ceg::InternalMove> evaluated_moves;
		std::mutex m_mutex;
		int current_index = 0;
		static constexpr int min_value = -10000000;
		static constexpr int max_value = 10000000;
		static constexpr int checkmate_value = -100000;
		uint64_t hashing_table[64][12];
		static constexpr int max_tt_entries = 10000000;
		long long timer;
		TTEntry* tt_table;
		MoveGenerator* move_generator = nullptr;
	};
}
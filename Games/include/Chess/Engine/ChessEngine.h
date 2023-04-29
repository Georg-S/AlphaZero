#pragma once
#include <vector>
#include <set>
#include <string>
#include <map>
#include "NegamaxAI.h"
#include "Utility.h"
#include "MoveGenerator.h"

namespace ceg
{
	struct Move
	{
		Move() = default;
		Move(int from_x, int from_y, int to_x, int to_y) : from_x(from_x), from_y(from_y), to_x(to_x), to_y(to_y) {};
		Move(const ceg::InternalMove& move)
		{
			from_x = move.from % 8;
			from_y = move.from / 8;
			to_x = move.to % 8;
			to_y = move.to / 8;
		}

		int from_x = 0;
		int from_y = 0;
		int to_x = 0;
		int to_y = 0;
	};

	enum class PieceColor { NONE = 0, BLACK, WHITE };
	enum class Piece { PAWN, BISHOP, KNIGHT, ROOK, QUEEN, KING };

	class ChessEngine
	{
	public:
		ChessEngine();
		BitBoard get_initial_board() const;
		static PieceColor get_next_player(PieceColor color);
		std::vector<ceg::Move> get_all_possible_moves(const ceg::BitBoard& board, ceg::PieceColor playerColor) const;
		std::vector<ceg::Move> get_all_possible_moves_for_piece(const ceg::BitBoard& board, int piece_x, int piece_y) const;
		std::pair<ceg::PieceColor, ceg::BitBoard> get_player_and_board_from_fen_string(const std::string& fen_string) const;
		ceg::Move get_ai_move(const ceg::BitBoard& board, PieceColor color, int depth); // depth is used for min and max depth, no timer termination will happen
		ceg::Move get_ai_move(const ceg::BitBoard& board, PieceColor color);
		void make_move(ceg::BitBoard& board, ceg::Move move) const;
		void make_move_with_auto_promo(ceg::BitBoard& board, ceg::Move move) const;
		bool is_move_valid(const ceg::BitBoard& board, const ceg::Move& move) const;
		bool has_pawn_reached_end_of_board(ceg::BitBoard& board) const;
		bool is_game_over(const ceg::BitBoard& board, PieceColor color) const;
		bool is_field_occupied(const ceg::BitBoard& board, int x, int y) const;
		PieceColor get_piece_color(const ceg::BitBoard& board, int x, int y) const;
		void set_piece(ceg::BitBoard& board, Piece piece, PieceColor color, int x, int y) const;
		bool is_check_mate(const ceg::BitBoard& board, PieceColor color) const;
		bool is_stale_mate(const ceg::BitBoard& board, PieceColor color) const;
		uint64_t perft(const std::string& FEN_str, int depth) const;
		std::set<std::string> perft_get_set(const std::string& FEN_str, int depth) const;
		std::map<std::string, int> perft_get_map(const std::string& FEN_str, int depth) const;
		void set_min_depth(int min_depth);
		void set_max_depth(int max_depth);
		void set_max_time_in_ms(long long ms);
	private:
		ceg::Move get_ai_move(const ceg::BitBoard& board, bool current_player_black, int min_depth, int max_depth, long long max_time_in_ms);
		BitBoard get_board_by_FEN_str(const std::string& FEN_str) const;
		bool to_bool(ceg::PieceColor color) const;
		ceg::InternalMove convert_to_internal(const ceg::Move& move) const;
		uint64_t perft(const std::string& FEN_str, int depth, std::set<std::string>* out_set, std::map<std::string, int>* out_map) const;
		uint64_t perft(const ceg::BitBoard& board, bool current_player_black, int depth, std::set<std::string>* out_set, std::map<std::string, int>* out_map) const;

		inline static const std::string initial_board_str = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -";
		std::unique_ptr<ceg::MoveGenerator> move_generator = nullptr;
		std::unique_ptr<NegamaxAI> ai = nullptr;
		int min_depth = 6;
		int max_depth = 40;
		long long max_time_in_milli_seconds = 3000;
	};
}
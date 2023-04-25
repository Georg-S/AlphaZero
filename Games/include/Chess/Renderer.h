#pragma once
#include <memory>
#include "Engine/ChessEngine.h"
#include "SDL/SDLHandler.h"
#include "RenderInformation.h"
#include "Constant.h"

namespace chess
{
	class Renderer
	{
	public:
		Renderer(ceg::ChessEngine* engine);
		Renderer(ceg::ChessEngine* engine, std::unique_ptr<SDLHandler> handler);
		void render(const RenderInformation& renderInfo);
		void render_promotion_selection(ceg::PieceColor color);
		void render_board(const std::string& FEN_board_str);
		void render_board(const ceg::BitBoard& board);
		int getWindowWidth() const;
		int getWindowHeight() const;
		bool is_quit() const;
		void quit();

	private:
		void render_chess_board();
		void render_pieces(const ceg::BitBoard& board);
		void render_piece(const std::vector<std::vector<char>>& fen_board, int x, int y);
		void render_pieces_with_selected_on_mouse_position(const RenderInformation& renderInfo);
		void render_previous_move(const ceg::Move& previousMove);
		void render_piece_on_mouse_position(char piece, int mouseX, int mouseY);
		void render_all_possible_moves_for_selected_piece(const ceg::BitBoard& board, int selected_x, int selected_y);
		void render_checkmate();
		void render_stalemate();
		std::string get_file_string(char fen_c) const;
		std::string get_file_string(uint32_t piece_type, ceg::PieceColor piece_color) const;
		std::string get_color_string(ceg::PieceColor color) const;
		std::string get_piece_type_string(char fen_c) const;

		int piece_width = window_width / board_width;
		int piece_height = window_height / board_height;
		std::unique_ptr<SDLHandler> sdl_handler = nullptr;
		ceg::ChessEngine* chess_engine = nullptr;
	};
}
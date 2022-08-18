#pragma once
#include <memory>
#include "Chess/ReducedChessAdapter.h"
#include "Other/Ai.h"
#include "Engine/ChessEngine.h"
#include "Renderer.h"
#include "SDL/Mouse.h"

class Chess
{
public:
	Chess();
	Chess(Ai* ai, ceg::PieceColor player_color);
	void game_loop();

private:
	void update_1_player_game();
	void update_2_player_game();
	void update_ai_move();
	void update_human_move();
	ceg::Move get_human_move();
	bool is_valid_move(const ceg::Move& move) const;
	bool is_valid_board_pos(int x, int y) const;
	void handle_promo_selection(ceg::BitBoard& board, int posx, int posy);
	ceg::Piece get_piece_from_promo_selection(int mouseX, int mouseY) const;
	void handle_game_over();
	int convert_mouse_position_x_to_board_position(int mouse_x) const;
	int convert_mouse_position_y_to_board_position(int mouse_y) const;

	std::unique_ptr<ceg::ChessEngine> engine = nullptr;
	std::unique_ptr<chess::Renderer> renderer;
	ceg::BitBoard board;
	std::unique_ptr<RenderInformation> render_info;
	int player_count = 2;
	bool game_over = false;
	ceg::PieceColor current_player = ceg::PieceColor::WHITE;
	ceg::PieceColor human_player_color;
	ceg::Move previous_move{-1, -1, -1, -1};
	ceg::Move pending_move{-1, -1, -1, -1};
	Mouse mouse;
	Ai* ai = nullptr;
};
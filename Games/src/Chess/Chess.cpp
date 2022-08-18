#include "Chess/Chess.h"

using namespace chess;

Chess::Chess()
{
	engine = std::make_unique<ceg::ChessEngine>();
	board = engine->get_initial_board();
	renderer = std::make_unique<Renderer>(engine.get());
	render_info = std::make_unique<RenderInformation>(board);
}

Chess::Chess(Ai* ai, ceg::PieceColor playerColor)
{
	engine = std::make_unique<ceg::ChessEngine>();
	board = engine->get_initial_board();
	renderer = std::make_unique<Renderer>(engine.get());
	render_info = std::make_unique<RenderInformation>(board);
	this->player_count = 1;
	this->ai = ai;
	human_player_color = playerColor;
}

void Chess::game_loop()
{
	while (!renderer->is_quit())
	{
		renderer->render(*render_info);
		if (game_over)
			continue;

		if (player_count == 1)
			update_1_player_game();
		else
			update_2_player_game();
	}
	renderer->quit();
}

void Chess::update_1_player_game()
{
	if (current_player == human_player_color)
		update_human_move();
	else
		update_ai_move();
}

void Chess::update_2_player_game()
{
	update_human_move();
}

void Chess::update_ai_move()
{
	int intMove = ai->getMove(ceg::to_FEN_string(board, current_player == ceg::PieceColor::BLACK), static_cast<int>(current_player));
	ceg::Move move = getMoveFromInt(intMove);

	if (!is_valid_move(move)) 
	{
		assert(!"Ai computed an invalid move");
		return;
	}
	engine->make_move_with_auto_promo(board, move);
	previous_move = move;
	render_info = std::make_unique<RenderInformation>(board, previous_move);

	current_player = engine->get_next_player(current_player);

	if (engine->is_game_over(board, current_player))
		handle_game_over();
}

void Chess::update_human_move()
{
	ceg::Move move = get_human_move();
	if (!is_valid_move(move))
		return;

	engine->make_move(board, move);
	previous_move = move;
	render_info = std::make_unique<RenderInformation>(board, previous_move);

	if (engine->has_pawn_reached_end_of_board(board))
		handle_promo_selection(board, move.to_x, move.to_y);

	current_player = engine->get_next_player(current_player);

	if (engine->is_game_over(board, current_player))
		handle_game_over();
}

ceg::Move Chess::get_human_move()
{
	mouse.update();
	ceg::Move move = ceg::Move{ -1, -1, -1, -1 };
	render_info = std::make_unique<RenderInformation>(board, previous_move);
	const bool piece_selected = (pending_move.from_x != -1) && (pending_move.from_y != -1);

	if (piece_selected)
	{
		render_info->selectedPieceX = pending_move.from_x;
		render_info->selectedPieceY = pending_move.from_y;
		render_info->mousePositionX = mouse.getMousePositionX();
		render_info->mousePositionY = mouse.getMousePositionY();
		render_info->previousMove = previous_move;
	}

	if (mouse.isRightPressed())
	{
		pending_move.from_x = -1;
		pending_move.from_y = -1;
		return move;
	}

	if (!mouse.isNewLeftClick())
		return move;

	const int boardX = convert_mouse_position_x_to_board_position(mouse.getMousePositionX());
	const int boardY = convert_mouse_position_y_to_board_position(mouse.getMousePositionY());

	if (!is_valid_board_pos(boardX, boardY))
		return move;

	if (!piece_selected)
	{
		bool is_valid_piece_for_current_player = engine->is_field_occupied(board, boardX, boardY) && (engine->get_piece_color(board, boardX, boardY) == current_player);
		if (is_valid_piece_for_current_player)
		{
			pending_move.from_x = boardX;
			pending_move.from_y = boardY;
		}
	}
	else
	{
		move = ceg::Move{ pending_move.from_x, pending_move.from_y, boardX, boardY };
		pending_move.from_x = -1;
		pending_move.from_y = -1;
	}

	return move;
}

bool Chess::is_valid_move(const ceg::Move& move) const
{
	if (!is_valid_board_pos(move.from_x, move.from_y) || !is_valid_board_pos(move.to_x, move.to_y))
		return false;
	if (!engine->is_field_occupied(board, move.from_x, move.from_y))
		return false;

	return engine->is_move_valid(board, move);
}

bool Chess::is_valid_board_pos(int x, int y) const
{
	return (x >= 0) && (x < board_width) && (y >= 0) && (y < board_height);
}

void Chess::handle_promo_selection(ceg::BitBoard& board, int posx, int posy)
{
	renderer->render_promotion_selection(current_player);

	bool validPieceSelected = false;

	while (!validPieceSelected)
	{
		mouse.update();

		if (!mouse.isNewLeftClick())
			continue;

		const int x = mouse.getMousePositionX() / (renderer->getWindowWidth() / 2);
		const int y = mouse.getMousePositionY() / (renderer->getWindowHeight() / 2);
		if (x > 1 || y > 1)
			continue;

		ceg::Piece piece = get_piece_from_promo_selection(x, y);

		engine->set_piece(board, piece, current_player, posx, posy);
		validPieceSelected = true;
	}
	render_info->board = board;
}

ceg::Piece Chess::get_piece_from_promo_selection(int x, int y) const
{
	if (x == 0 && y == 0)
		return ceg::Piece::QUEEN;
	else if (x == 1 && y == 0)
		return ceg::Piece::ROOK;
	else if (x == 0 && y == 1)
		return ceg::Piece::KNIGHT;
	else
		return ceg::Piece::BISHOP;
}

void Chess::handle_game_over()
{
	game_over = true;
	if (engine->is_check_mate(board, current_player))
		render_info->check_mate = true;
	else if (engine->is_stale_mate(board, current_player))
		render_info->stale_mate = true;
}

int Chess::convert_mouse_position_x_to_board_position(int mouse_x) const
{
	return mouse_x / (renderer->getWindowWidth() / board_width);
}

int Chess::convert_mouse_position_y_to_board_position(int mouse_y) const
{
	return mouse_y / (renderer->getWindowHeight() / board_height);
}

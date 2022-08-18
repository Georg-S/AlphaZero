#include "Chess/Renderer.h"

using namespace chess;

static const std::string basePath = "Images/Chess/";

Renderer::Renderer(ceg::ChessEngine* engine)
{
	sdl_handler = std::make_unique<SDLHandler>(window_width, window_height, true);
	sdl_handler->start("Chess");
	chess_engine = engine;
}

Renderer::Renderer(ceg::ChessEngine* engine, std::unique_ptr<SDLHandler> handler)
{
	chess_engine = engine;
	sdl_handler = std::move(handler);
}

void Renderer::render(const RenderInformation& renderInfo)
{
	sdl_handler->clear();
	render_chess_board();

	if (renderInfo.previousMove.from_x != -1)
		render_previous_move(renderInfo.previousMove);

	if (renderInfo.selectedPieceY == -1 || renderInfo.selectedPieceX == -1) 
	{
		render_pieces(renderInfo.board);
	}
	else 
	{
		render_pieces_with_selected_on_mouse_position(renderInfo);
		render_all_possible_moves_for_selected_piece(renderInfo.board, renderInfo.selectedPieceX, renderInfo.selectedPieceY);
	}

	if (renderInfo.check_mate)
		render_checkmate();
	else if (renderInfo.stale_mate)
		render_stalemate();

	sdl_handler->update();
}

void Renderer::render_board(const std::string& FEN_board_str)
{
	sdl_handler->clear();
	render_chess_board();
	render_pieces(FEN_board_str);
	sdl_handler->update();
}

void Renderer::render_chess_board()
{
	sdl_handler->createAndPushBackRenderElement(basePath + "Board.png", 0, 0, window_width, window_height);
}

void Renderer::render_pieces(const ceg::BitBoard& board)
{
	auto fen_board = board.get_fen_char_representation();

	for (int x = 0; x < board_width; x++)
	{
		for (int y = 0; y < board_height; y++)
		{
			if (fen_board[x][y] == '-')
				continue;

			render_piece(fen_board, x, y);
		}
	}
}

void Renderer::render_piece(const std::vector<std::vector<char>>& fen_board, int x, int y)
{
	std::string fileString = get_file_string(fen_board[x][y]);
	sdl_handler->createAndPushBackRenderElement(fileString, piece_width * x, piece_height * y, piece_width, piece_height);
}

std::string Renderer::get_file_string(char fen_c) const
{
	ceg::PieceColor color = islower(fen_c) ? ceg::PieceColor::BLACK : ceg::PieceColor::WHITE;

	return basePath + get_piece_type_string(tolower(fen_c)) + "_" + get_color_string(color) + ".png";
}

std::string Renderer::get_file_string(uint32_t piece_type, ceg::PieceColor piece_color) const
{
	return basePath + get_piece_type_string(piece_type) + "_" + get_color_string(piece_color) + ".png";
}

std::string Renderer::get_color_string(ceg::PieceColor color) const
{
	if (color == ceg::PieceColor::WHITE)
		return "white";
	else
		return "black";
}

std::string Renderer::get_piece_type_string(char fen_c) const
{
	switch (fen_c)
	{
	case 'p':		return "Pawn";
	case 'n':		return "Knight";
	case 'q':		return "Queen";
	case 'k':		return "King";
	case 'b':		return "Bishop";
	case 'r':		return "Rook";
	default:		assert(0);
	}
	return "";
}

void Renderer::render_pieces_with_selected_on_mouse_position(const RenderInformation& renderInfo)
{
	auto fen_board = renderInfo.board.get_fen_char_representation();
	char foreGroundPiece;

	for (int x = 0; x < board_width; x++)
	{
		for (int y = 0; y < board_height; y++)
		{
			if (x == renderInfo.selectedPieceX && y == renderInfo.selectedPieceY)
				foreGroundPiece = fen_board[x][y];
			else if (fen_board[x][y] != '-')
				render_piece(fen_board, x, y);
		}
	}
	render_piece_on_mouse_position(foreGroundPiece, renderInfo.mousePositionX, renderInfo.mousePositionY);
}

void Renderer::render_piece_on_mouse_position(char piece, int mouseX, int mouseY)
{
	std::string fileString = get_file_string(piece);
	sdl_handler->createAndPushBackRenderElement(fileString, mouseX - (piece_width / 2), mouseY - (piece_height / 2), piece_width, piece_height);
}

void Renderer::render_all_possible_moves_for_selected_piece(const ceg::BitBoard& board, int selected_x, int selected_y)
{
	auto possible_moves = chess_engine->get_all_possible_moves_for_piece(board, selected_x, selected_y);

	for (const auto& move : possible_moves) 
	{

		sdl_handler->createAndPushBackRenderElement(basePath + "PossibleMove.png", piece_width * move.to_x, piece_height * move.to_y
			, piece_width, piece_height);
	}
}

void Renderer::render_checkmate()
{
	sdl_handler->createAndPushBackRenderElement(basePath + "Checkmate/Checkmate.png", 0, window_height / 3, window_width, window_height / 4);
}

void Renderer::render_stalemate()
{
	sdl_handler->createAndPushBackRenderElement(basePath + "Stalemate/Stalemate.png", 0, window_height / 3, window_width, window_height / 4);
}

void Renderer::render_previous_move(const ceg::Move& previousMove)
{
	sdl_handler->createAndPushBackRenderElement(basePath + "PreviousMove.png", piece_width * previousMove.from_x, piece_height * previousMove.from_y
		, piece_width, piece_height);

	sdl_handler->createAndPushBackRenderElement(basePath + "PreviousMove.png", piece_width * previousMove.to_x, piece_height * previousMove.to_y
		, piece_width, piece_height);
}

int Renderer::getWindowWidth() const
{
	return window_width;
}

int Renderer::getWindowHeight() const
{
	return window_height;
}

void Renderer::render_promotion_selection(ceg::PieceColor color)
{
	sdl_handler->clear();
	sdl_handler->createAndPushBackRenderElement(basePath + "background.png", 0, 0, window_width, window_height);

	std::string queenStr = get_file_string('q', color);
	std::string rookStr = get_file_string('r', color);
	std::string knightStr = get_file_string('n', color);
	std::string  bishopStr = get_file_string('b', color);

	sdl_handler->createAndPushBackRenderElement(queenStr, 0, 0, window_width / 2, window_height / 2);
	sdl_handler->createAndPushBackRenderElement(rookStr, window_width / 2, 0, window_width / 2, window_height / 2);
	sdl_handler->createAndPushBackRenderElement(knightStr, 0, window_height / 2, window_width / 2, window_height / 2);
	sdl_handler->createAndPushBackRenderElement(bishopStr, window_width / 2, window_height / 2, window_width / 2, window_height / 2);

	sdl_handler->update();
}

bool Renderer::is_quit() const
{
	return sdl_handler->isExit();
}

void Renderer::quit()
{
	sdl_handler->close();
}

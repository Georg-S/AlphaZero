#include "Chess/Engine/BitBoard.h"

ceg::BitBoard::BitBoard(const std::string& FEN_str)
{
	auto splitted = string_split(FEN_str, " ");
	assert(!splitted.empty());

	if (splitted.size() == 1)
		set_board(splitted.front());
	if (splitted.size() == 3)
		set_board(splitted.front(), splitted[2]);
	if (splitted.size() >= 4)
		set_board(splitted.front(), splitted[2], splitted[3]);
}

ceg::BitBoard::BitBoard(const std::string& FEN_pieces_str, const std::string& FEN_castling_str, const std::string& FEN_en_passant_str)
{
	set_board(FEN_pieces_str, FEN_castling_str, FEN_en_passant_str);
}

void ceg::BitBoard::move_piece(Pieces* pieces, const InternalMove& move)
{
	auto piece = get_ptr_to_piece(pieces, move.from);

	clear_bit(*piece, move.from);
	set_bit(*piece, move.to);

	clear_bit(pieces->occupied, move.from);
	set_bit(pieces->occupied, move.to);
}

void ceg::BitBoard::clear_bit_for_pieces(Pieces* pieces, int bit_index)
{
	clear_bit(pieces->bishops, bit_index);
	//		clear_bit(pieces->king, bit_index); // King must not be beaten in chess
	clear_bit(pieces->knights, bit_index);
	clear_bit(pieces->pawns, bit_index);
	clear_bit(pieces->queens, bit_index);
	clear_bit(pieces->rooks, bit_index);
	clear_bit(pieces->occupied, bit_index);
}

void ceg::BitBoard::clear_bits_at_position(int bit_index)
{
	clear_bit_for_pieces(&black_pieces, bit_index);
	clear_bit_for_pieces(&white_pieces, bit_index);
	update_occupied();
}

uint64_t* ceg::BitBoard::get_ptr_to_piece(Pieces* pieces, int bit_index) const
{
	if (ceg::is_bit_set(pieces->pawns, bit_index)) return &(pieces->pawns);
	if (ceg::is_bit_set(pieces->queens, bit_index)) return &(pieces->queens);
	if (ceg::is_bit_set(pieces->bishops, bit_index)) return &(pieces->bishops);
	if (ceg::is_bit_set(pieces->rooks, bit_index)) return &(pieces->rooks);
	if (ceg::is_bit_set(pieces->knights, bit_index)) return &(pieces->knights);
	if (ceg::is_bit_set(pieces->king, bit_index)) return &(pieces->king);

	assert(!"No piece found at the bit index");
	return nullptr;
}

void ceg::BitBoard::print_board_to_console() const
{
	for (int y = 0; y < ceg::board_height; y++)
	{
		for (int x = 0; x < ceg::board_width; x++)
		{
			std::cout << get_field_char(x, y) << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void ceg::BitBoard::set_board(const std::string& FEN_pieces_str, const std::string& FEN_castling_str, const std::string& FEN_en_passant_str)
{
	auto splitted = ceg::string_split(FEN_pieces_str, "/");
	assert(splitted.size() == 8);

	for (int y = 0; y < ceg::board_height; y++)
	{
		int x_index = 0;
		for (char c : splitted[y])
		{
			if (ceg::is_number(c))
			{
				x_index += c - '0';
				continue;
			}

			set_piece_by_FEN_char(c, x_index, y);
			x_index++;
		}
	}
	update_occupied();

	if (FEN_castling_str != "")
		set_castling(FEN_castling_str);
	if (FEN_en_passant_str != "")
		set_en_passant(FEN_en_passant_str);

}

void ceg::BitBoard::set_castling(const std::string& FEN_castling_str)
{
	for (char c : FEN_castling_str)
	{
		if (c == 'k')
			set_bit(black_pieces.castling, 7, 0);
		if (c == 'q')
			set_bit(black_pieces.castling, 0, 0);
		if (c == 'K')
			set_bit(white_pieces.castling, 7, 7);
		if (c == 'Q')
			set_bit(white_pieces.castling, 0, 7);
	}
}

void ceg::BitBoard::set_en_passant(const std::string& FEN_str)
{
	int x = -1;
	int y = -1;
	for (char c : FEN_str)
	{
		c = tolower(c);
		if (c >= 'a' && c <= 'h')
			x = c - 'a';
		if (c >= '1' && c <= '8')
			y = '8' - c;	// Convert from "official" coordinates to internal board coordinates
	}
	if (x != -1 && y != -1)
		set_bit(en_passant_mask, x, y);
}

void ceg::BitBoard::update_occupied()
{
	black_pieces.occupied = black_pieces.bishops | black_pieces.king | black_pieces.knights | black_pieces.pawns | black_pieces.queens | black_pieces.rooks;
	white_pieces.occupied = white_pieces.bishops | white_pieces.king | white_pieces.knights | white_pieces.pawns | white_pieces.queens | white_pieces.rooks;
	occupied = black_pieces.occupied | white_pieces.occupied;
}

std::vector<std::vector<char>> ceg::BitBoard::get_fen_char_representation() const
{
	std::vector<std::vector<char>> result(8, std::vector<char>(8));
	for (int y = 0; y < ceg::board_height; y++)
	{
		for (int x = 0; x < ceg::board_width; x++)
		{
			result[x][y] = get_field_char(x, y);
		}
	}

	return result;
}

bool ceg::BitBoard::is_occupied(int x, int y) const
{
	return is_bit_set(occupied, x, y);
}

char ceg::BitBoard::get_field_char(int x, int y) const
{
	if (!is_occupied(x, y))
		return '-';

	return get_FEN_char(x, y);
}

char ceg::BitBoard::get_FEN_char(int x, int y) const
{
	if (ceg::is_bit_set(black_pieces.pawns, x, y)) return 'p';
	if (ceg::is_bit_set(white_pieces.pawns, x, y)) return 'P';
	if (ceg::is_bit_set(black_pieces.bishops, x, y)) return 'b';
	if (ceg::is_bit_set(white_pieces.bishops, x, y)) return 'B';
	if (ceg::is_bit_set(black_pieces.knights, x, y)) return 'n';
	if (ceg::is_bit_set(white_pieces.knights, x, y)) return 'N';
	if (ceg::is_bit_set(black_pieces.rooks, x, y)) return 'r';
	if (ceg::is_bit_set(white_pieces.rooks, x, y)) return 'R';
	if (ceg::is_bit_set(black_pieces.queens, x, y)) return 'q';
	if (ceg::is_bit_set(white_pieces.queens, x, y)) return 'Q';
	if (ceg::is_bit_set(black_pieces.king, x, y)) return 'k';
	if (ceg::is_bit_set(white_pieces.king, x, y)) return 'K';
	assert(!"Couldn't get FEN char, no piece set at position");
	return ' ';
}

std::string ceg::BitBoard::getPiecesFENString() const
{
	std::string piecesString;
	for (int y = 0; y < 8; y++)
	{
		int counter = 0;
		for (int x = 0; x < 8; x++)
		{
			if (!ceg::is_bit_set(occupied, x, y))
			{
				counter++;
				continue;
			}

			if (counter != 0)
			{
				piecesString += std::to_string(counter);
				counter = 0;
			}

			char fen_c = get_FEN_char(x, y);
			piecesString += fen_c;
		}

		if (counter != 0)
		{
			piecesString += std::to_string(counter);
			counter = 0;
		}

		if (y != 7)
			piecesString += "/";
	}

	return piecesString;
}

std::string ceg::BitBoard::getCastlingFENString() const
{
	std::string castlingString;
	if (is_bit_set(white_pieces.castling, 7, 7))
		castlingString += 'K';
	if (is_bit_set(white_pieces.castling, 0, 7))
		castlingString += 'Q';
	if (is_bit_set(black_pieces.castling, 7, 0))
		castlingString += 'k';
	if (is_bit_set(black_pieces.castling, 0, 0))
		castlingString += 'q';

	if (castlingString.empty())
		return "-";

	return castlingString;
}

std::string ceg::BitBoard::getEnPassantFENString() const
{
	if (en_passant_mask == 0)
		return "-";

	int index = get_bit_index_lsb(en_passant_mask);
	int x = index % 8;
	int y = 8 - (index / 8); // Convert from internal board coordinates to "official" coordinates

	char column = 'a' + x;
	std::string rowStr = std::to_string(y);

	return column + rowStr;
}

void ceg::BitBoard::set_piece_by_FEN_char(char c, int x, int y)
{
	bool black = islower(c);
	c = tolower(c);
	switch (c)
	{
	case 'p':
		if (black)
			set_bit(black_pieces.pawns, x, y);
		else
			set_bit(white_pieces.pawns, x, y);
		break;
	case 'n':
		if (black)
			set_bit(black_pieces.knights, x, y);
		else
			set_bit(white_pieces.knights, x, y);
		break;
	case 'b':
		if (black)
			set_bit(black_pieces.bishops, x, y);
		else
			set_bit(white_pieces.bishops, x, y);
		break;
	case 'r':
		if (black)
			set_bit(black_pieces.rooks, x, y);
		else
			set_bit(white_pieces.rooks, x, y);
		break;
	case 'q':
		if (black)
			set_bit(black_pieces.queens, x, y);
		else
			set_bit(white_pieces.queens, x, y);
		break;
	case 'k':
		if (black)
			set_bit(black_pieces.king, x, y);
		else
			set_bit(white_pieces.king, x, y);
		break;
	default:
		assert(!"Invalid character for FEN string");
		return;
	}
}

std::string ceg::to_FEN_string(const BitBoard& board, bool currentPlayerBlack)
{
	auto piecesString = board.getPiecesFENString();
	auto castlingString = board.getCastlingFENString();
	auto enPassantString = board.getEnPassantFENString();

	auto result = piecesString;
	if (currentPlayerBlack)
		result += " b";
	else
		result += " w";

	result += " " + castlingString;
	result += " " + enPassantString;

	return result;
}

#include "Chess/Engine/NegamaxAI.h"

ceg::NegamaxAI::NegamaxAI(MoveGenerator* move_generator)
{
	srand(time(NULL));
	init_hashing_table();
	tt_table = new TTEntry[max_tt_entries];
	this->move_generator = move_generator;
}

ceg::NegamaxAI::~NegamaxAI()
{
	delete[] tt_table;
}

ceg::InternalMove ceg::NegamaxAI::get_move(const ceg::BitBoard& board, bool color_is_black, int depth)
{
	return iterative_deepening(board, color_is_black, depth, depth, -1);
}

ceg::InternalMove ceg::NegamaxAI::get_move(const ceg::BitBoard& board, bool color_is_black, int min_depth, int max_depth, long long time_in_ms) 
{
	return iterative_deepening(board, color_is_black, min_depth, max_depth, time_in_ms);
}

ceg::InternalMove ceg::NegamaxAI::iterative_deepening(const ceg::BitBoard& board, bool color_is_black, int min_depth, int max_depth, long long max_time_in_ms)
{
	assert(min_depth >= 0);
	auto possible_moves = move_generator->get_all_possible_moves(board, color_is_black);
	std::vector<ceg::InternalMove> last_fully_evaluated_moves;
	auto buf_timer = ceg::get_current_time_in_ms() + max_time_in_ms;

	for (int i = 0; i <= max_depth; i++)
	{
		if (i <= min_depth)
			timer = -1; // A timer set to -1 will not expire
		else
			timer = buf_timer;

		possible_moves = get_evaluated_moves(board, color_is_black, i, possible_moves);
		const bool timer_expired = ceg::get_current_time_in_ms() > buf_timer;

		if (i <= min_depth || !timer_expired) 
			last_fully_evaluated_moves = possible_moves;

		if (timer_expired && (i > min_depth))
			break;

		std::sort(possible_moves.begin(), possible_moves.end(), operator>);
	}
	auto best_moves = get_best_moves(last_fully_evaluated_moves);

	return get_random_move(best_moves);
}

void ceg::NegamaxAI::init_hashing_table()
{
	for (int i = 0; i < board_width * board_height; i++)
	{
		for (int piece_type = 0; piece_type < 12; piece_type++)
			hashing_table[i][piece_type] = ALZ::getRandomLongNumber(0LL, std::numeric_limits<uint64_t>::max());
	}
}

void ceg::NegamaxAI::hash_piece(uint64_t& current_hash, uint64_t piece, int table_index) const
{
	while (piece != 0)
	{
		int index = ceg::get_bit_index_lsb(piece);
		ceg::reset_lsb(piece);

		current_hash ^= hashing_table[index][table_index];
	}
}

uint64_t ceg::NegamaxAI::hash_board(const ceg::BitBoard& board, bool color_is_black) const
{
	constexpr uint64_t max_uint64_t = 0xFFFFFFFFFFFFFFFF;
	uint64_t hash = 0;

	hash_piece(hash, board.white_pieces.pawns, 0);
	hash_piece(hash, board.white_pieces.knights, 2);
	hash_piece(hash, board.white_pieces.bishops, 4);
	hash_piece(hash, board.white_pieces.rooks, 6);
	hash_piece(hash, board.white_pieces.queens, 8);
	hash_piece(hash, board.white_pieces.king, 10);

	hash_piece(hash, board.black_pieces.pawns, 1);
	hash_piece(hash, board.black_pieces.knights, 3);
	hash_piece(hash, board.black_pieces.bishops, 5);
	hash_piece(hash, board.black_pieces.rooks, 7);
	hash_piece(hash, board.black_pieces.queens, 9);
	hash_piece(hash, board.black_pieces.king, 11);

	if (color_is_black)
		return hash ^ max_uint64_t;

	return hash;
}

static bool get_next_player(bool black)
{
	return !black;
}

std::vector<ceg::InternalMove> ceg::NegamaxAI::get_evaluated_moves(const ceg::BitBoard& board, bool color_is_black, int depth)
{
	std::vector<ceg::InternalMove> evaluated_moves;
	auto possible_moves = move_generator->get_all_possible_moves(board, color_is_black);
	for (auto move : possible_moves)
	{
		ceg::BitBoard copy_board = board;
		move_generator->make_move_with_auto_promotion(copy_board, move);

		move.value = evaluate_board_negamax(copy_board, get_next_player(color_is_black), depth, min_value, max_value);
		evaluated_moves.push_back(move);
	}
	return evaluated_moves;
}

std::vector<ceg::InternalMove> ceg::NegamaxAI::get_evaluated_moves_multi_threaded(const ceg::BitBoard& board, bool color_is_black, int depth)
{
	auto possible_moves = move_generator->get_all_possible_moves(board, color_is_black);
	return get_evaluated_moves(board, color_is_black, depth, possible_moves);
}

std::vector<ceg::InternalMove> ceg::NegamaxAI::get_evaluated_moves(const ceg::BitBoard& board, bool color_is_black, int depth, const std::vector<ceg::InternalMove>& possible_moves)
{
	const auto processor_count = std::thread::hardware_concurrency();
	const int thread_count = std::max((unsigned int)1, processor_count);
	std::vector<std::thread> thread_pool;

	evaluated_moves.clear();
	current_index = 0;

	for (int i = 0; i < thread_count; i++)
		thread_pool.push_back(std::thread(&NegamaxAI::eval_multi_threaded, this, board, color_is_black, possible_moves, depth));

	for (auto& thread : thread_pool)
		thread.join();

	return evaluated_moves;
}

void ceg::NegamaxAI::eval_multi_threaded(const ceg::BitBoard& board, bool color_is_black, const std::vector<ceg::InternalMove>& possible_moves, int depth)
{
	m_mutex.lock();
	int move_index = current_index;
	current_index++;
	m_mutex.unlock();

	while (move_index < possible_moves.size())
	{
		ceg::InternalMove move = possible_moves[move_index];
		ceg::BitBoard copy_board = board;
		move_generator->make_move_with_auto_promotion(copy_board, move);
		move.value = evaluate_board_negamax(copy_board, get_next_player(color_is_black), depth, min_value, max_value);

		m_mutex.lock();
		evaluated_moves.push_back(move);
		move_index = current_index;
		current_index++;
		m_mutex.unlock();
	}
}

ceg::InternalMove ceg::NegamaxAI::get_random_move(const std::vector<ceg::InternalMove>& moves) const
{
	assert(moves.size());
	if (moves.size() == 0)
		return ceg::InternalMove();

	return moves[rand() % moves.size()];
}

static void set_move_to_front(std::vector<ceg::InternalMove>& moves, const ceg::InternalMove& move)
{
	for (int i = 0; i < moves.size(); i++)
	{
		const auto& buf = moves[i];
		if (buf.from == move.from && buf.to == move.to)
		{
			std::swap(moves[0], moves[i]);
			return;
		}
	}
}

static int get_piece_MVV_LVA_index(const ceg::BitBoard& board, int index)
{
	if (!ceg::is_bit_set(board.occupied, index))
		return 6;

	if (ceg::is_bit_set(board.black_pieces.pawns | board.white_pieces.pawns, index)) return 5;
	if (ceg::is_bit_set(board.black_pieces.knights | board.white_pieces.knights, index)) return 4;
	if (ceg::is_bit_set(board.black_pieces.bishops | board.white_pieces.bishops, index)) return 3;
	if (ceg::is_bit_set(board.black_pieces.rooks | board.white_pieces.rooks, index)) return 2;
	if (ceg::is_bit_set(board.black_pieces.queens | board.white_pieces.queens, index)) return 1;
	if (ceg::is_bit_set(board.black_pieces.king | board.white_pieces.king, index)) return 0;

	assert(!"Invalid index for get_piece_MVV_LVA");
	return 6;
}

static void sort_moves_by_MVV_LVA(const ceg::BitBoard& board, std::vector<ceg::InternalMove>& moves)
{
	for (int i = 0; i < moves.size(); i++)
	{
		const ceg::InternalMove& move = moves[i];
		int from_index = get_piece_MVV_LVA_index(board, move.from);
		int to_index = get_piece_MVV_LVA_index(board, move.to);

		moves[i].value = ceg::MVV_LVA[to_index][from_index];
	}
	std::sort(moves.begin(), moves.end(), ceg::operator>);
}

void ceg::NegamaxAI::sort_possible_moves(const ceg::BitBoard& board, std::vector<ceg::InternalMove>& moves, bool color_is_black, const ceg::InternalMove& tt_move) const
{
	sort_moves_by_MVV_LVA(board, moves);

	if (tt_move.from != -1)
		set_move_to_front(moves, tt_move);
}


int ceg::NegamaxAI::evaluate_board_negamax(const ceg::BitBoard& board, bool color_is_black, int depth, int alpha, int beta)
{
	const int initial_alpha = alpha;
	const uint64_t hash = hash_board(board, color_is_black);

	int index = hash % max_tt_entries;
	ceg::InternalMove tt_move{ -1 };
	auto& entry = tt_table[index];
	{
		std::scoped_lock lock(entry.mut);
		if ((entry.hash != 0) && (entry.hash == hash) && (entry.depth >= depth))
		{
			tt_move = entry.best_move;
			if (entry.type == TTEntry::type::EXACT)
				return -entry.value;
			else if (entry.type == TTEntry::type::LOWER)
				alpha = std::max(alpha, entry.value);
			else if (entry.type == TTEntry::type::UPPER)
				beta = std::min(beta, entry.value);

			if (alpha >= beta)
				return -entry.value;
		}
	}

	auto state = move_generator->get_state_information(board, color_is_black);
	if ((state.check_counter >= 1) && (state.possible_moves.size() == 0)) 
		return -(checkmate_value - depth);
	if (state.possible_moves.size() == 0)
		return 0;

	if (depth == 0)
		return -static_board_evaluation(board, color_is_black);

	auto& possible_moves = state.possible_moves;
	sort_possible_moves(board, possible_moves, color_is_black, tt_move);
	assert(possible_moves.size());

	ceg::InternalMove best_move{};
	int move_value = min_value;
	for (const auto& move : possible_moves)
	{
		ceg::BitBoard copy_board = board;
		move_generator->make_move_with_auto_promotion(copy_board, move);

		int eval_value = evaluate_board_negamax(copy_board, get_next_player(color_is_black), depth - 1, -beta, -alpha);
		if (eval_value > move_value)
		{
			best_move = move;
			move_value = eval_value;
		}
		alpha = std::max(alpha, move_value);
		if (alpha >= beta)
			break;
		if ((timer != -1) && (ceg::get_current_time_in_ms() > timer))
			return 0;
	}

	if ((timer != -1) && (ceg::get_current_time_in_ms() > timer))
		return 0;

	std::scoped_lock lock(entry.mut);
	if (depth > entry.depth)
	{
		entry.value = move_value;
		entry.depth = depth;
		entry.hash = hash;
		entry.best_move = best_move;
		if (move_value <= initial_alpha)
			entry.type = TTEntry::type::UPPER;
		else if (move_value >= beta)
			entry.type = TTEntry::type::LOWER;
		else
			entry.type = TTEntry::type::EXACT;
	}

	return -move_value;
}

int ceg::NegamaxAI::static_board_evaluation(const ceg::BitBoard& board, bool current_player_black) const
{
	int black_val = get_pieces_value(board, board.black_pieces, true);
	int white_val = get_pieces_value(board, board.white_pieces, false);

	if (current_player_black)
		return black_val - white_val;
	else
		return white_val - black_val;
}

int ceg::NegamaxAI::get_pieces_value(const ceg::BitBoard& board, Pieces pieces, bool black_pieces) const
{
	constexpr int pawn_value = 100;
	constexpr int knight_value = 320;
	constexpr int bishop_value = 330;
	constexpr int rook_value = 500;
	constexpr int queen_value = 900;
	constexpr int king_value = 10000;

	int result = 0;
	while (pieces.pawns)
	{
		int index = ceg::get_bit_index_lsb(pieces.pawns);
		reset_lsb(pieces.pawns);

		result += pawn_value;
		if (black_pieces)
			result += black_pawn_table[index];
		else
			result += white_pawn_table[index];
	}

	while (pieces.bishops)
	{
		int index = ceg::get_bit_index_lsb(pieces.bishops);
		reset_lsb(pieces.bishops);

		result += bishop_value;
		if (black_pieces)
			result += black_bishop_table[index];
		else
			result += white_bishop_table[index];
	}

	while (pieces.knights)
	{
		int index = ceg::get_bit_index_lsb(pieces.knights);
		reset_lsb(pieces.knights);

		result += knight_value;
		if (black_pieces)
			result += black_knight_table[index];
		else
			result += white_knight_table[index];
	}

	while (pieces.rooks)
	{
		int index = ceg::get_bit_index_lsb(pieces.rooks);
		reset_lsb(pieces.rooks);

		result += rook_value;
		if (black_pieces)
			result += black_rook_table[index];
		else
			result += white_rook_table[index];
	}

	while (pieces.queens)
	{
		int index = ceg::get_bit_index_lsb(pieces.queens);
		reset_lsb(pieces.queens);

		result += queen_value;
		if (black_pieces)
			result += black_queen_table[index];
		else
			result += white_queen_table[index];
	}

	while (pieces.king)
	{
		int index = ceg::get_bit_index_lsb(pieces.king);
		reset_lsb(pieces.king);

		bool endgame = is_end_game(board);
		result += king_value;
		if (black_pieces)
		{
			if (endgame)
				result += black_king_end_game_table[index];
			else
				result += black_king_early_game_table[index];
		}
		else
		{
			if (endgame)
				result += black_king_end_game_table[index];
			else
				result += white_king_early_game_table[index];
		}
	}

	return result;
}

std::vector<ceg::InternalMove> ceg::NegamaxAI::get_best_moves(const std::vector<ceg::InternalMove>& moves) const
{
	std::vector<ceg::InternalMove> best_moves;
	assert(moves.size());
	if (moves.size() == 0)
		return best_moves;

	auto max_element = std::max_element(moves.begin(), moves.end());

	int highest_value = (*(max_element)).value;
	for (const auto& eval_move : moves)
	{
		if (eval_move.value == highest_value)
			best_moves.push_back(eval_move);
	}

	return best_moves;
}

int ceg::NegamaxAI::get_piece_count(uint64_t piece) const
{
	int count = 0;
	while (piece)
	{
		ceg::reset_lsb(piece);
		count++;
	}
	return count;
}

bool ceg::NegamaxAI::is_end_game(const BitBoard& board) const
{
	int black_queens = get_piece_count(board.black_pieces.queens);
	int white_queens = get_piece_count(board.white_pieces.queens);
	if ((black_queens + white_queens) == 0)
		return true;
	if ((white_queens >= 2) || (black_queens >= 2))
		return false;


	int black_light_pieces = get_piece_count(board.black_pieces.bishops) + get_piece_count(board.black_pieces.rooks)
		+ get_piece_count(board.black_pieces.knights);
	int white_light_pieces = get_piece_count(board.white_pieces.bishops) + get_piece_count(board.white_pieces.rooks)
		+ get_piece_count(board.white_pieces.knights);

	return (black_light_pieces <= 1 || black_queens == 0) && (white_light_pieces <= 1 || white_queens == 0);
}

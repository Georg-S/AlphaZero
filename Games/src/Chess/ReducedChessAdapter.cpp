#include "Chess/ReducedChessAdapter.h"

ceg::Move chess::getMoveFromInt(int move) 
{
	ceg::Move res;
	int from = move % 64;
	int to = move / 64;
	
	res.from_x = from % 8;
	res.from_y = from / 8;
	res.to_x = to % 8;
	res.to_y = to / 8;

	return res;
}

int chess::getIntFromMove(const ceg::Move& move) 
{
	int from = move.from_x + (8 * move.from_y);
	int to = (move.to_x + (8 * move.to_y)) * 64;

	return from | to;
}

ReducedChessAdapter::ReducedChessAdapter()
{
	chessEngine = std::make_unique<ceg::ChessEngine>();
}

std::vector<int> ReducedChessAdapter::getAllPossibleMoves(const std::string& state, int currentPlayer)
{
	ceg::BitBoard board(state);

	auto moves = chessEngine->get_all_possible_moves(board, ceg::PieceColor(currentPlayer));

	auto result = std::vector<int>();
	result.reserve(moves.size());

	for(const auto& move : moves) 
		result.emplace_back(chess::getIntFromMove(move));

	return result;
}

int ReducedChessAdapter::getInitialPlayer()
{
	return static_cast<int>(ceg::PieceColor::WHITE);
}

std::string ReducedChessAdapter::getInitialGameState()
{
	return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -";
}

int ReducedChessAdapter::getPlayerWon(const std::string& state)
{
	ceg::BitBoard board(state);
	if (chessEngine->is_check_mate(board, ceg::PieceColor::WHITE))
		return static_cast<int>(ceg::PieceColor::BLACK);
	if (chessEngine->is_check_mate(board, ceg::PieceColor::BLACK))
		return static_cast<int>(ceg::PieceColor::WHITE);

	return static_cast<int>(ceg::PieceColor::NONE);
}

int ReducedChessAdapter::getNextPlayer(int currentPlayer)
{
	auto color = ceg::PieceColor(currentPlayer);
	assert(color != ceg::PieceColor::NONE);

	return static_cast<int>(chessEngine->get_next_player(color));
}

std::string ReducedChessAdapter::makeMove(const std::string& state, int move, int currentPlayer)
{
	ceg::BitBoard board(state);
	auto mMove = chess::getMoveFromInt(move);
	chessEngine->make_move(board, mMove);

	int nextPlayer = getNextPlayer(currentPlayer);
	auto result = ceg::to_FEN_string(board, ceg::PieceColor(nextPlayer) == ceg::PieceColor::BLACK);

	return result;
}

static void convertPiecesToTensor(uint64_t pieces, at::Tensor destination)
{
	while (pieces) 
	{
		int index = ceg::get_bit_index_lsb(pieces);
		ceg::reset_lsb(pieces);

		int x = index % 8;
		int y = index / 8;

		assert(y <= 8);

		destination[x][y] = 1.f;
	}
}

static void setPiecesInTensor(const ceg::Pieces& pieces, at::Tensor destination, int startIndex) 
{
	convertPiecesToTensor(pieces.pawns, destination[startIndex]);
	convertPiecesToTensor(pieces.rooks, destination[startIndex + 1]);
	convertPiecesToTensor(pieces.knights, destination[startIndex + 2]);
	convertPiecesToTensor(pieces.bishops, destination[startIndex + 3]);
	convertPiecesToTensor(pieces.queens, destination[startIndex + 4]);
	convertPiecesToTensor(pieces.king, destination[startIndex + 5]);
	//convertPiecesToTensor(pieces.castling, destination[startIndex + 6]);
}

torch::Tensor ReducedChessAdapter::convertStateToNeuralNetInput(const std::string& state, int currentPlayer,
	torch::Device device)
{
	constexpr int perPlayerSize = 6;
	ceg::BitBoard board(state);
	ceg::PieceColor currentColor = ceg::PieceColor(currentPlayer);
	torch::Tensor result = torch::zeros({ 1,12,8,8 });

	if (currentColor == ceg::PieceColor::WHITE) 
	{
		setPiecesInTensor(board.white_pieces, result[0], 0);
		setPiecesInTensor(board.black_pieces, result[0], perPlayerSize);
	}
	else
	{
		setPiecesInTensor(board.black_pieces, result[0], 0);
		setPiecesInTensor(board.white_pieces, result[0], perPlayerSize);
	}

	result = result.to(device);

	return result;
}

bool ReducedChessAdapter::isGameOver(const std::string& state)
{
	auto [player, board] = chessEngine->get_player_and_board_from_fen_string(state);

	return chessEngine->is_game_over(board, player);
}

int ReducedChessAdapter::gameOverReward(const std::string& state, int currentPlayer)
{
	auto color = ceg::PieceColor(currentPlayer);
	auto otherColor = chessEngine->get_next_player(color);
	ceg::BitBoard board(state);

	if (chessEngine->is_check_mate(board, color))
		return -1;
	else if (chessEngine->is_check_mate(board, otherColor))
		return 1;

	return 0;
}

int ReducedChessAdapter::getActionCount() const
{
	return m_actionCount;
}

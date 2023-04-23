#include "Chess/ChessAdapter.h"

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

ChessAdapter::ChessAdapter()
{
	chessEngine = std::make_unique<ceg::ChessEngine>();
}

std::vector<int> ChessAdapter::getAllPossibleMoves(const std::string& state, int currentPlayer)
{
	ceg::BitBoard board(state);

	auto moves = chessEngine->get_all_possible_moves(board, ceg::PieceColor(currentPlayer));

	auto result = std::vector<int>();
	result.reserve(moves.size());

	for(const auto& move : moves) 
		result.emplace_back(chess::getIntFromMove(move));

	return result;
}

int ChessAdapter::getInitialPlayer()
{
	return static_cast<int>(ceg::PieceColor::WHITE);
}

std::string ChessAdapter::getInitialGameState()
{
	return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -";
}

int ChessAdapter::getPlayerWon(const std::string& state)
{
	ceg::BitBoard board(state);
	if (chessEngine->is_check_mate(board, ceg::PieceColor::WHITE))
		return static_cast<int>(ceg::PieceColor::BLACK);
	if (chessEngine->is_check_mate(board, ceg::PieceColor::BLACK))
		return static_cast<int>(ceg::PieceColor::WHITE);

	return static_cast<int>(ceg::PieceColor::NONE);
}

int ChessAdapter::getNextPlayer(int currentPlayer)
{
	auto color = ceg::PieceColor(currentPlayer);
	assert(color != ceg::PieceColor::NONE);

	return static_cast<int>(chessEngine->get_next_player(color));
}

std::string ChessAdapter::makeMove(const std::string& state, int move, int currentPlayer)
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

static void setPiecesInTensor(const ceg::Pieces& pieces, uint64_t en_passant, at::Tensor destination, size_t startIndex)
{
	convertPiecesToTensor(pieces.pawns, destination[startIndex]);
	convertPiecesToTensor(pieces.rooks, destination[startIndex + 1]);
	convertPiecesToTensor(pieces.knights, destination[startIndex + 2]);
	convertPiecesToTensor(pieces.bishops, destination[startIndex + 3]);
	convertPiecesToTensor(pieces.queens, destination[startIndex + 4]);
	convertPiecesToTensor(pieces.king, destination[startIndex + 5]);
	// Put castling and en_passant into the same plane to save some memory
	convertPiecesToTensor(pieces.castling, destination[startIndex + 6]);	
	convertPiecesToTensor(en_passant, destination[startIndex + 6]);
}

torch::Tensor ChessAdapter::convertStateToNeuralNetInput(const std::string& state, int currentPlayer)
{

	torch::Tensor result = torch::zeros({ 1,14,8,8 });
	convertStateToNeuralNetInput(state, currentPlayer, result[0]);

	return result;
}

void ChessAdapter::convertStateToNeuralNetInput(const std::string& state, int currentPlayer, torch::Tensor outTensor)
{
	constexpr int perPlayerSize = 7;
	ceg::BitBoard board(state);
	ceg::PieceColor currentColor = ceg::PieceColor(currentPlayer);

	outTensor.zero_();
	if (currentColor == ceg::PieceColor::WHITE)
	{
		setPiecesInTensor(board.white_pieces, board.en_passant_mask, outTensor, 0);
		setPiecesInTensor(board.black_pieces, 0LL, outTensor, perPlayerSize);
	}
	else
	{
		setPiecesInTensor(board.black_pieces, board.en_passant_mask, outTensor, 0);
		setPiecesInTensor(board.white_pieces, 0LL, outTensor, perPlayerSize);
	}
}

bool ChessAdapter::isGameOver(const std::string& state)
{
	auto [player, board] = chessEngine->get_player_and_board_from_fen_string(state);

	return chessEngine->is_game_over(board, player);
}

int ChessAdapter::gameOverReward(const std::string& state, int currentPlayer)
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

int ChessAdapter::getActionCount() const
{
	return m_actionCount;
}

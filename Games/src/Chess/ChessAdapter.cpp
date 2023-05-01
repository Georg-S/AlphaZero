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

ChessAdapter::ChessAdapter()
{
	chessEngine = std::make_unique<ceg::ChessEngine>();
}

int ChessAdapter::getInitialPlayer() const
{
	return static_cast<int>(ceg::PieceColor::WHITE);
}

int ChessAdapter::getNextPlayer(int currentPlayer) const
{
	auto color = ceg::PieceColor(currentPlayer);
	assert(color != ceg::PieceColor::NONE);

	return static_cast<int>(chessEngine->get_next_player(color));
}

ChessAdapter::GameState ChessAdapter::getInitialGameState() const
{
	static const std::string initialGameStateStr = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -";
	return GameState(ceg::BitBoard(initialGameStateStr), static_cast<int>(ceg::PieceColor::WHITE));
}

int ChessAdapter::getPlayerWon(const GameState& gameState) const
{
	if (chessEngine->is_check_mate(gameState.board, ceg::PieceColor::WHITE))
		return static_cast<int>(ceg::PieceColor::BLACK);
	if (chessEngine->is_check_mate(gameState.board, ceg::PieceColor::BLACK))
		return static_cast<int>(ceg::PieceColor::WHITE);

	return static_cast<int>(ceg::PieceColor::NONE);
}

torch::Tensor ChessAdapter::convertStateToNeuralNetInput(const GameState& state, int currentPlayer) const
{
	torch::Tensor result = torch::zeros({ 1,14,8,8 });
	convertStateToNeuralNetInput(state, currentPlayer, result[0]);

	return result;
}

void ChessAdapter::convertStateToNeuralNetInput(const GameState& state, int currentPlayer, torch::Tensor outTensor) const
{
	constexpr int perPlayerSize = 7;
	const auto& board = state.board;
	const ceg::PieceColor currentColor = ceg::PieceColor(currentPlayer);

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

std::vector<int> ChessAdapter::getAllPossibleMoves(const GameState& gameState, int currentPlayer) const
{
	const auto moves = chessEngine->get_all_possible_moves(gameState.board, ceg::PieceColor(currentPlayer));

	auto result = std::vector<int>();
	result.reserve(moves.size());

	for (const auto& move : moves)
		result.emplace_back(chess::getIntFromMove(move));

	return result;
}

int ChessAdapter::gameOverReward(const GameState& state, int currentPlayer) const
{
	auto color = ceg::PieceColor(currentPlayer);
	auto otherColor = chessEngine->get_next_player(color);

	if (chessEngine->is_check_mate(state.board, color))
		return -1;
	else if (chessEngine->is_check_mate(state.board, otherColor))
		return 1;

	return 0;
}

bool ChessAdapter::isGameOver(const GameState& state) const
{
	return chessEngine->is_game_over(state.board, ceg::PieceColor(state.currentPlayer));
}

ChessAdapter::GameState ChessAdapter::makeMove(GameState state, int move, int currentPlayer) const
{
	auto mMove = chess::getMoveFromInt(move);
	chessEngine->make_move_with_auto_promo(state.board, mMove);
	state.currentPlayer = static_cast<int>(getNextPlayer(currentPlayer));

	return state;
}

int ChessAdapter::getActionCount() const
{
	return m_actionCount;
}

ChessAdapter::GameState ChessAdapter::getGameStateFromString(const std::string str, int currentPlayer) const
{
	auto [player, board] = chessEngine->get_player_and_board_from_fen_string(str);
	return GameState(board, static_cast<int>(player));
}

std::string ChessAdapter::getStringFromGameState(const GameState& board) const
{
	return ceg::to_FEN_string(board.board, ceg::PieceColor(board.currentPlayer) == ceg::PieceColor::BLACK);
}

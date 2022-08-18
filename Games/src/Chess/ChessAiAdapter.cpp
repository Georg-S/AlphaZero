#include "Chess/ChessAiAdapter.h"

chess::NegaMaxAi::NegaMaxAi(int depth)
	: m_depth(depth)
{
	m_engine = std::make_unique<ceg::ChessEngine>();
}

int chess::NegaMaxAi::getMove(const std::string& state, int playerColor)
{
	auto board = ceg::BitBoard(state);
	auto move = m_engine->get_ai_move(board, ceg::PieceColor(playerColor), m_depth);

	return chess::getIntFromMove(move);
}
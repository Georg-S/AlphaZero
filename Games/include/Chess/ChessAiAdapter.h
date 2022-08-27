#pragma once
#include "Other/Ai.h"
#include "Chess/ChessAdapter.h"
#include "Chess/Engine/ChessEngine.h"
#include "Chess/Engine/NegamaxAI.h"

namespace chess
{
	class NegaMaxAi : public Ai
	{
	public:
		explicit NegaMaxAi(int depth);
		int getMove(const std::string& state, int playerColor) override;

	private:
		int m_depth;
		std::unique_ptr<ceg::ChessEngine> m_engine;
	};
}
#ifndef DEEPREINFORCEMENTLEARNING_REDUCEDCHESSADAPTER_H
#define DEEPREINFORCEMENTLEARNING_REDUCEDCHESSADAPTER_H

#include <Game.h>
#include "Chess/Engine/ChessEngine.h"

namespace chess
{
	ceg::Move getMoveFromInt(int move);
	int getIntFromMove(const ceg::Move& move);
}

class ChessAdapter : public Game
{
public:
	struct GameState
	{
		ceg::BitBoard board;
		int currentPlayer;

		GameState() = default;
		GameState(ceg::BitBoard board, int currentPlayer) 
			: board(std::move(board))
			, currentPlayer(currentPlayer)
		{
		}

		friend bool operator<(const GameState& lhs, const GameState& rhs)
		{
			const uint64_t lhsArr[] =
			{
				lhs.board.white_pieces.bishops,
				lhs.board.white_pieces.castling,
				lhs.board.white_pieces.king,
				lhs.board.white_pieces.knights,
				lhs.board.white_pieces.pawns,
				lhs.board.white_pieces.queens,
				lhs.board.white_pieces.rooks,
				lhs.board.black_pieces.bishops,
				lhs.board.black_pieces.castling,
				lhs.board.black_pieces.king,
				lhs.board.black_pieces.knights,
				lhs.board.black_pieces.pawns,
				lhs.board.black_pieces.queens,
				lhs.board.black_pieces.rooks,
				lhs.board.en_passant_mask,
				lhs.currentPlayer
			};

			const uint64_t rhsArr[] =
			{
				rhs.board.white_pieces.bishops,
				rhs.board.white_pieces.castling,
				rhs.board.white_pieces.king,
				rhs.board.white_pieces.knights,
				rhs.board.white_pieces.pawns,
				rhs.board.white_pieces.queens,
				rhs.board.white_pieces.rooks,
				rhs.board.black_pieces.bishops,
				rhs.board.black_pieces.castling,
				rhs.board.black_pieces.king,
				rhs.board.black_pieces.knights,
				rhs.board.black_pieces.pawns,
				rhs.board.black_pieces.queens,
				rhs.board.black_pieces.rooks,
				rhs.board.en_passant_mask,
				rhs.currentPlayer
			};

			const auto res = memcmp(lhsArr, rhsArr, std::size(lhsArr) * sizeof(lhsArr[0]));
			if (res < 0)
				return true;
			return false;
		}
	};

	ChessAdapter();
	std::vector<int> getAllPossibleMoves(const std::string& state, int currentPlayer) override;
	std::vector<int> getAllPossibleMoves(const GameState& gameState, int currentPlayer) const;
	int getInitialPlayer() override;
	std::string getInitialGameState() override;
	int getPlayerWon(const std::string& state) override;
	int getPlayerWon(const GameState& gameState) const;
	int getNextPlayer(int currentPlayer) override;
	std::string makeMove(const std::string& state, int move, int currentPlayer) override;
	GameState makeMove(GameState state, int move, int currentPlayer); // TODO make const
	torch::Tensor convertStateToNeuralNetInput(const std::string& state, int currentPlayer) override;
	torch::Tensor convertStateToNeuralNetInput(const GameState& state, int currentPlayer) const;
	void convertStateToNeuralNetInput(const std::string& state, int currentPlayer, torch::Tensor outTensor) override;
	void convertStateToNeuralNetInput(const GameState& state, int currentPlayer, torch::Tensor outTensor) const;
	bool isGameOver(const std::string& state) override;
	bool isGameOver(const GameState& state) const;
	int gameOverReward(const std::string& state, int currentPlayer) override;
	int gameOverReward(const GameState& state, int currentPlayer) const;
	int getActionCount() const override;

private:
	static constexpr int m_actionCount = 4096;
	std::unique_ptr<ceg::ChessEngine> chessEngine;
};


#endif //DEEPREINFORCEMENTLEARNING_REDUCEDCHESSADAPTER_H

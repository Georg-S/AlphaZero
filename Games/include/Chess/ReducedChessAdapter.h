#ifndef DEEPREINFORCEMENTLEARNING_REDUCEDCHESSADAPTER_H
#define DEEPREINFORCEMENTLEARNING_REDUCEDCHESSADAPTER_H

#include <Game.h>
#include "Pieces/Queen.h"
#include "GameLogic.h"
#include "Board.h"

// The Reduced Adapter for Chess
// Reduced means, the adapter doesn't know about Castling and En passant
// This is mainly a simplification and hopefully speeds up learning

class ReducedChessAdapter : public Game {
public:
	ReducedChessAdapter();

	std::vector<int> getAllPossibleMoves(const std::string& state, int currentPlayer) override;
	int getInitialPlayer() override;
	std::string getInitialGameState() override;
	int getPlayerWon(const std::string& state) override;
	int getNextPlayer(int currentPlayer) override;
	std::string makeMove(const std::string& state, int move, int currentPlayer) override;
	torch::Tensor convertStateToNeuralNetInput(const std::string& state, int currentPlayer, torch::Device device = torch::kCPU) override;
	bool isGameOver(const std::string& state) override;
	int gameOverReward(const std::string& state, int currentPlayer) override;

	static std::string convertStateToString(chess::Board& board, int currentPlayer);
	static chess::Board convertStateStringToBoard(std::string state);

private:
	static std::string convertBoardToString(chess::Board& board);
	static chess::Board convertStringToBoard(std::string state);
	static std::string convertPiecesToString(const chess::Board& board);
	static std::string convertColorToString(const chess::Board& board);
	static chess::Piece* getPieceFromChar(const char& pieceChar, const chess::PieceColor& color);
	static chess::PieceColor getCurrentPlayerFromState(const std::string& state);
	static void convertPiecesToTensor(chess::Board& board, const chess::PieceColor& color, at::Tensor destination, const char& pieceChar);
	void replacePawnsWithQueens(chess::Board& board);
	void replacePawnsWithQueens(chess::Board& board, int row);

	bool bothKingsStillThere(const chess::Board& board);

	const std::string initialState = "1RHBQKBHRPPPPPPPP--------------------------------PPPPPPPPRHBQKBHR2222222222222222--------------------------------1111111111111111";
	const int actionCount = 4096;
};


#endif //DEEPREINFORCEMENTLEARNING_REDUCEDCHESSADAPTER_H

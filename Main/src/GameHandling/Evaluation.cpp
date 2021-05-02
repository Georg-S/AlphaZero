#include <ConnectFour/MiniMaxAi.h>
#include "GameHandling/Evaluation.h"

EvalResult Evaluation::eval(Ai* neuralNetAi, Ai* miniMaxAi, Game* game, int numberEvalGames)
{
	int currentNetColor = 1;
	int draws = 0;
	int wins = 0;
	int losses = 0;

	Ai* currentPlayer1 = neuralNetAi;
	Ai* currentPlayer2 = miniMaxAi;

	for (int i = 0; i < numberEvalGames; i++) {
		int winner = Evaluation::runGame(currentPlayer1, currentPlayer2, game);

		std::swap(currentPlayer1, currentPlayer2);

		if (winner == 0)
			draws++;
		else if (winner == currentNetColor)
			wins++;
		else
			losses++;

		currentNetColor = (currentNetColor % 2 + 1);
	}
	EvalResult result;
	result.wins = wins;
	result.losses = losses;
	result.draws = draws;

	return result;
	//    std::cout << "Games Played: " << numberEvalGames << " Wins: " << wins << " Loss: " << lose << " Draws: " << draws << std::endl;
}

int Evaluation::runGame(Ai* ai1, Ai* ai2, Game* game)
{
	std::string state = game->getInitialGameState();
	int currentPlayer = game->getInitialPlayer();

	while (!game->isGameOver(state)) {
		int move = -1;
		if (currentPlayer == 1)
			move = ai1->getMove(state, currentPlayer);
		else
			move = ai2->getMove(state, currentPlayer);

		state = game->makeMove(state, move, currentPlayer);
		currentPlayer = game->getNextPlayer(currentPlayer);
	}
	return game->getPlayerWon(state);
}

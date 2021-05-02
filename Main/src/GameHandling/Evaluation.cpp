#include "GameHandling/Evaluation.h"

Evaluation::Evaluation(torch::DeviceType device, int mctsCount) : device(device), mctsCount(mctsCount)
{
}

EvalResult Evaluation::eval(Ai* neuralNetAi, Ai* miniMaxAi, Game* game, int numberEvalGames)
{
	int currentNetColor = 1;
	int draws = 0;
	int wins = 0;
	int losses = 0;

	Ai* currentPlayer1 = neuralNetAi;
	Ai* currentPlayer2 = miniMaxAi;

	for (int i = 0; i < numberEvalGames; i++)
	{
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
}

int Evaluation::runGame(Ai* ai1, Ai* ai2, Game* game)
{
	std::string state = game->getInitialGameState();
	int currentPlayer = game->getInitialPlayer();

	while (!game->isGameOver(state))
	{
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


EvalResult Evaluation::evalMultiThreaded(MultiThreadingNeuralNetManager* threadManager, Ai* miniMaxAi, Game* game, int numberEvalGames)
{
	std::vector<std::thread> threadPool;
	EvalResult result;
	int currentNetColor = game->getInitialPlayer();
	int gamesToPlay = numberEvalGames;
	for (int i = 0; i < threadManager->getThreadCount(); i++)
	{
		threadPool.push_back(std::thread(&Evaluation::selfPlayMultiThreadGames, this, threadManager, miniMaxAi, game,
			std::ref(result), std::ref(gamesToPlay), std::ref(currentNetColor), rand()));
		currentNetColor = game->getNextPlayer(currentNetColor);
	}

	for (auto& thread : threadPool)
		thread.join();

	return result;
}

void Evaluation::selfPlayMultiThreadGames(MultiThreadingNeuralNetManager* threadManager, Ai* miniMaxAi, Game* game,
	EvalResult& result, int& gamesToPlay, int& color, int seed)
{
	srand(seed);

	int myColor = 0;
	while (true)
	{
		mut.lock();
		if (gamesToPlay == 0)
		{
			threadManager->safeDecrementActiveThreads();
			mut.unlock();
			return;
		}
		else
		{
			myColor = color;
			color = game->getNextPlayer(color);
			gamesToPlay--;
			mut.unlock();
		}

		int winner = runGameMultiThreaded(threadManager, miniMaxAi, game, myColor);

		mut.lock();
		if (winner == 0)
			result.draws++;
		else if (winner == myColor)
			result.wins++;
		else
			result.losses++;
		mut.unlock();

	}
}

int Evaluation::runGameMultiThreaded(MultiThreadingNeuralNetManager* threadManager, Ai* minMaxAi, Game* game, int neuralNetColor)
{
	std::string state = game->getInitialGameState();
	int currentPlayer = game->getInitialPlayer();
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(game->getActionCount());

	while (!game->isGameOver(state))
	{
		int move = -1;
		if (currentPlayer == neuralNetColor)
		{
			mcts.multiThreadedSearch(mctsCount, state, game, currentPlayer, threadManager, device);
			std::vector<float> probs = mcts.getProbabilities(state);
			move = std::max_element(probs.begin(), probs.end()) - probs.begin();
		}
		else
		{
			mut.lock();
			move = minMaxAi->getMove(state, currentPlayer);
			mut.unlock();
		}
		state = game->makeMove(state, move, currentPlayer);
		currentPlayer = game->getNextPlayer(currentPlayer);
	}

	return game->getPlayerWon(state);
}


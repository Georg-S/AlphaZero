#include "GameHandling/Evaluation.h"

Evaluation::Evaluation(torch::DeviceType device, int mctsCount) 
	: m_mctsCount(mctsCount)
	, device(device)
{
}

EvalResult Evaluation::evalMultiThreaded(MultiThreadingNeuralNetManager* threadManager, Ai* miniMaxAi, Game* game, int numberEvalGames)
{
	std::vector<std::thread> threadPool;
	EvalResult result;

	m_currentColor = game->getInitialPlayer();
	m_gamesToPlay = numberEvalGames;

	for (int i = 0; i < threadManager->getThreadCount(); i++)
		threadPool.push_back(std::thread(&Evaluation::selfPlayMultiThreadGames, this, threadManager, miniMaxAi, game, &result));

	for (auto& thread : threadPool)
		thread.join();

	return result;
}

void Evaluation::selfPlayMultiThreadGames(MultiThreadingNeuralNetManager* threadManager, Ai* miniMaxAi, Game* game,
	EvalResult* outResult)
{
	int myColor = 0;
	while (true)
	{
		m_mut.lock();
		if (m_gamesToPlay == 0)
		{
			threadManager->safeDecrementActiveThreads();
			m_mut.unlock();
			return;
		}

		myColor = m_currentColor;
		m_currentColor = game->getNextPlayer(m_currentColor);
		m_gamesToPlay--;
		m_mut.unlock();

		int winner = runGameMultiThreaded(threadManager, miniMaxAi, game, myColor);

		m_mut.lock();
		if (winner == 0)
			outResult->draws++;
		else if (winner == myColor)
			outResult->wins++;
		else
			outResult->losses++;
		m_mut.unlock();
	}
}

int Evaluation::runGameMultiThreaded(MultiThreadingNeuralNetManager* threadManager, Ai* minMaxAi, Game* game, int neuralNetColor)
{
	std::string state = game->getInitialGameState();
	int currentPlayer = game->getInitialPlayer();
	// One difference between single threaded and multi threaded eval
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(game->getActionCount());

	while (!game->isGameOver(state))
	{
		int move = -1;
		if (currentPlayer == neuralNetColor)
		{
			mcts.multiThreadedSearch(m_mctsCount, state, game, currentPlayer, threadManager, device);
			std::vector<float> probs = mcts.getProbabilities(state);
			move = std::max_element(probs.begin(), probs.end()) - probs.begin();
		}
		else
		{
			m_mut.lock();
			move = minMaxAi->getMove(state, currentPlayer);
			m_mut.unlock();
		}
		state = game->makeMove(state, move, currentPlayer);
		currentPlayer = game->getNextPlayer(currentPlayer);
	}

	return game->getPlayerWon(state);
}

#include "GameHandling/Evaluation.h"

Evaluation::Evaluation(torch::DeviceType device, int mctsCount)
	: m_mctsCount(mctsCount)
	, m_device(device)
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
	MonteCarloTreeSearch mcts = MonteCarloTreeSearch(game->getActionCount(), m_device);

	while (!game->isGameOver(state))
	{
		int move = -1;
		if (currentPlayer == neuralNetColor)
		{
			mcts.multiThreadedSearch(m_mctsCount, state, game, currentPlayer, threadManager);
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

namespace
{
	struct GameState
	{
		GameState(std::string currentState, int currentPlayer, int actionCount, torch::DeviceType device)
			: currentState(currentState)
			, currentPlayer(currentPlayer)
			, mcts(MonteCarloTreeSearch(actionCount, device))
		{
		}

		std::string currentState;
		int currentPlayer;
		bool continueMcts = false;
		int netBufferIndex = -1;
		int currentStep = 0;
		int color = -1;
		MonteCarloTreeSearch mcts;
	};
}

EvalResult Evaluation::eval(NeuralNetwork* net, Ai* miniMaxAi, Game* game, int batchSize, EvalResult& outEval, int mctsCount, int numberEvalGames)
{
	auto netInputBuffer = NeuralNetInputBuffer(m_device);
	int playerBuf = game->getInitialPlayer();
	auto currentStatesData = std::vector<GameState>(batchSize, { game->getInitialGameState(), game->getInitialPlayer(), game->getActionCount(), m_device });
	for (auto& currentState : currentStatesData)
	{
		currentState.color = playerBuf;
		playerBuf = game->getNextPlayer(playerBuf);
	}

	/*
	Use a vector of pointers to the gamedata for iterating,
	then all of the game data can be destroyed at once.
	This should improve the performance quite a bit
	*/
	auto currentStates = std::vector<GameState*>();
	for (auto& elem : currentStatesData)
		currentStates.emplace_back(&elem);

	while (!currentStates.empty())
	{
		//std::cout << currentStates.back()->currentStep << std::endl;
		netInputBuffer.calculateOutput(net);

		for (size_t i = 0; i < currentStates.size(); i++)
		{
			auto& currentStateObj = *(currentStates[i]);
			auto& mcts = currentStateObj.mcts;
			auto& currentState = currentStateObj.currentState;
			auto& continueMcts = currentStateObj.continueMcts;
			auto& currentPlayer = currentStateObj.currentPlayer;
			auto& netInputIndex = currentStateObj.netBufferIndex;
			auto& currentStep = currentStateObj.currentStep;
			const auto& colorBuf = currentStateObj.color;

			int move = -1;

			if (currentPlayer != colorBuf)
			{
				move = miniMaxAi->getMove(currentState, currentPlayer);
			}
			else
			{
				// Maybe add game too long here? But currently not needed for Tic-Tac-Toe and Connect-Four

				if (!continueMcts)
				{
					continueMcts = mcts.specialSearch(currentState, game, currentPlayer, mctsCount);
				}
				else
				{
					auto [valueTens, probTens] = netInputBuffer.getOutput(netInputIndex);
					continueMcts = mcts.continueSpecialSearch(currentState, game, currentPlayer, valueTens, probTens);
				}

				if (continueMcts)
				{
					netInputIndex = netInputBuffer.addToInput(mcts.getExpansionNeuralNetInput(game, m_device));
					continue;
				}

				std::vector<float> probs = mcts.getProbabilities(currentState);
				move = ALZ::getMaxElementIndex(probs);
			}

			currentState = game->makeMove(currentState, move, currentPlayer);
			currentPlayer = game->getNextPlayer(currentPlayer);
			currentStep++;

			if (game->isGameOver(currentState))
			{
				int playerWon = game->getPlayerWon(currentState);
				//std::cout << currentState << std::endl;
				//std::cout << "Player won: " << playerWon << " neuralNetColor: " << colorBuf << std::endl;
				if (playerWon == colorBuf)
					outEval.wins++;
				else if (playerWon == 0)
					outEval.draws++;
				else
					outEval.losses++;

				currentStates.erase(currentStates.begin() + i);
				i--;
			}
		}
	}
}

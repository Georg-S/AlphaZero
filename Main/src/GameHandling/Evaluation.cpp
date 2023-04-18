#include "GameHandling/Evaluation.h"

Evaluation::Evaluation(torch::DeviceType device, int mctsCount)
	: m_mctsCount(mctsCount)
	, m_device(device)
{
}

namespace
{
	struct GameState
	{
		GameState(std::string currentState, int currentPlayer, int actionCount, torch::DeviceType device, Game* game)
			: currentState(currentState)
			, currentPlayer(currentPlayer)
			, mcts(MonteCarloTreeSearch(actionCount, device, game))
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

void Evaluation::eval(NeuralNetwork* net, Ai* miniMaxAi, Game* game, int batchSize, EvalResult& outEval, int numberEvalGames)
{
	auto netInputBuffer = MonteCarloTreeSearchCache(m_device, game);
	int playerBuf = game->getInitialPlayer();
	auto currentStatesData = std::vector<GameState>(batchSize, { game->getInitialGameState(), game->getInitialPlayer(), game->getActionCount(), m_device, game });
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
		netInputBuffer.expand(net);

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
					continueMcts = mcts.startSearchWithoutExpansion(currentState, game, currentPlayer, m_mctsCount);
				else
					continueMcts = mcts.expandAndContinueSearchWithoutExpansion(currentState, game, currentPlayer);

				if (continueMcts)
					continue;

				auto probs = mcts.getProbabilities(currentState);
				move = ALZ::getBestAction(probs);
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

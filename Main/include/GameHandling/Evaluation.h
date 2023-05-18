#ifndef DEEPREINFORCEMENTLEARNING_EVALUATION_H
#define DEEPREINFORCEMENTLEARNING_EVALUATION_H

#include <string>
#include <iostream>
#include <Other/Ai.h>
#include <Other/NeuralNetAi.h>
#include <TicTacToe/TicTacToeAdapter.h>

struct EvalResult
{
	int wins = 0;
	int losses = 0;
	int draws = 0;
};


template <typename GameState, typename Game, bool mockExpansion>
class Evaluation
{
public:
	struct SelfPlayState
	{
		SelfPlayState(GameState currentState, int currentPlayer, torch::DeviceType device
			, Game* game, MonteCarloTreeSearchCache<GameState, Game, mockExpansion>* cache)
			: currentState(currentState)
			, currentPlayer(currentPlayer)
			, mcts(MonteCarloTreeSearch<GameState, Game, mockExpansion>(cache, game, device))
		{
		}

		GameState currentState;
		int currentPlayer;
		bool continueMcts = false;
		int netBufferIndex = -1;
		int currentStep = 0;
		int color = -1;
		MonteCarloTreeSearch<GameState, Game, mockExpansion> mcts;
	};

	Evaluation(torch::DeviceType device, int mctsCount, Game* game)
		: m_device(device)
		, m_mctsCount(mctsCount)
		, m_game(game)
	{
	}

	void eval(NeuralNetwork* net, Ai* miniMaxAi, int batchSize, EvalResult& outEval, int numberEvalGames)
	{
		auto netInputBuffer = MonteCarloTreeSearchCache<GameState, Game, mockExpansion>(m_device, m_game, net);
		int playerBuf = m_game->getInitialPlayer();
		auto currentStatesData = std::vector<SelfPlayState>(batchSize, { m_game->getInitialGameState(), m_game->getInitialPlayer(), m_device, m_game, &netInputBuffer });
		
		for (auto& currentState : currentStatesData)
		{
			currentState.color = playerBuf;
			playerBuf = m_game->getNextPlayer(playerBuf);
		}

		/*
		Use a vector of pointers to the gamedata for iterating,
		then all of the game data can be destroyed at once.
		This should improve the performance quite a bit
		*/
		auto currentStates = std::vector<SelfPlayState*>();
		for (auto& elem : currentStatesData)
			currentStates.emplace_back(&elem);

		while (!currentStates.empty())
		{
			netInputBuffer.convertToNeuralInput();
			netInputBuffer.expand();

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
					auto currentStateStr = m_game->getStringFromGameState(currentState);
					move = miniMaxAi->getMove(currentStateStr, currentPlayer);
				}
				else
				{
					// Maybe add game too long here? But currently not needed for Tic-Tac-Toe and Connect-Four

					if (!continueMcts)
						continueMcts = mcts.startSearchWithoutExpansion(currentState, currentPlayer, m_mctsCount);
					else
						continueMcts = mcts.expandAndContinueSearchWithoutExpansion(currentState, currentPlayer);

					if (continueMcts)
						continue;

					auto probs = mcts.getProbabilities(currentState);
					move = ALZ::getBestAction(probs);
				}

				currentState = m_game->makeMove(currentState, move, currentPlayer);
				currentPlayer = m_game->getNextPlayer(currentPlayer);
				currentStep++;

				if (m_game->isGameOver(currentState))
				{
					int playerWon = m_game->getPlayerWon(currentState);
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

private:
	Game* m_game;
	int m_gamesToPlay = 0;
	int m_currentColor = 0;
	int m_mctsCount = 50;
	torch::DeviceType m_device = torch::kCPU;
};


#endif //DEEPREINFORCEMENTLEARNING_EVALUATION_H

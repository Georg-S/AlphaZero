#ifndef DEEPREINFORCEMENTLEARNING_EVALUATION_H
#define DEEPREINFORCEMENTLEARNING_EVALUATION_H

#include <string>
#include <iostream>
#include <Other/Ai.h>
#include <Other/NeuralNetAi.h>
#include <TicTacToe/TicTacToeAdapter.h>

//struct EvalResult
//{
//	int wins = 0;
//	int losses = 0;
//	int draws = 0;
//};
//
//
//template <typename GameState, typename Game, bool mockExpansion>
//class Evaluation
//{
//public:
//	struct SelfPlayState
//	{
//		SelfPlayState(GameState currentState, int currentPlayer, torch::DeviceType device
//			, Game* game, MonteCarloTreeSearchCacheT<GameState, Game, mockExpansion>* cache)
//			: currentState(currentState)
//			, currentPlayer(currentPlayer)
//			, mcts(MonteCarloTreeSearchT<GameState, Game, mockExpansion>(cache, game, device))
//		{
//		}
//
//		GameState currentState;
//		int currentPlayer;
//		bool continueMcts = false;
//		int netBufferIndex = -1;
//		int currentStep = 0;
//		int color = -1;
//		MonteCarloTreeSearchT<GameState, Game, mockExpansion> mcts;
//	};
//
//	Evaluation(torch::DeviceType device, int mctsCount)
//		: m_mctsCount(mctsCount)
//		, m_device(device)
//	{
//	}
//
//	void eval(NeuralNetwork* net, Ai* miniMaxAi, Game* game, int batchSize, EvalResult& outEval, int numberEvalGames)
//	{
//		auto netInputBuffer = MonteCarloTreeSearchCacheT<GameState, Game, mockExpansion>(m_device, game);
//		int playerBuf = game->getInitialPlayer();
//		auto currentStatesData = std::vector<SelfPlayState>(batchSize, { game->getInitialGameState(), game->getInitialPlayer(), game->getActionCount(), m_device, game, &netInputBuffer });
//		for (auto& currentState : currentStatesData)
//		{
//			currentState.color = playerBuf;
//			playerBuf = game->getNextPlayer(playerBuf);
//		}
//
//		/*
//		Use a vector of pointers to the gamedata for iterating,
//		then all of the game data can be destroyed at once.
//		This should improve the performance quite a bit
//		*/
//		auto currentStates = std::vector<SelfPlayState*>();
//		for (auto& elem : currentStatesData)
//			currentStates.emplace_back(&elem);
//
//		while (!currentStates.empty())
//		{
//			netInputBuffer.convertToNeuralInput();
//			netInputBuffer.expand(net);
//
//			for (size_t i = 0; i < currentStates.size(); i++)
//			{
//				auto& currentStateObj = *(currentStates[i]);
//				auto& mcts = currentStateObj.mcts;
//				auto& currentState = currentStateObj.currentState;
//				auto& continueMcts = currentStateObj.continueMcts;
//				auto& currentPlayer = currentStateObj.currentPlayer;
//				auto& netInputIndex = currentStateObj.netBufferIndex;
//				auto& currentStep = currentStateObj.currentStep;
//				const auto& colorBuf = currentStateObj.color;
//
//				int move = -1;
//
//				if (currentPlayer != colorBuf)
//				{
//					move = miniMaxAi->getMove(currentState, currentPlayer);
//				}
//				else
//				{
//					// Maybe add game too long here? But currently not needed for Tic-Tac-Toe and Connect-Four
//
//					if (!continueMcts)
//						continueMcts = mcts.startSearchWithoutExpansion(currentState, currentPlayer, m_mctsCount);
//					else
//						continueMcts = mcts.expandAndContinueSearchWithoutExpansion(currentState, currentPlayer);
//
//					if (continueMcts)
//						continue;
//
//					auto probs = mcts.getProbabilities(currentState);
//					move = ALZ::getBestAction(probs);
//				}
//
//				currentState = game->makeMove(currentState, move, currentPlayer);
//				currentPlayer = game->getNextPlayer(currentPlayer);
//				currentStep++;
//
//				if (game->isGameOver(currentState))
//				{
//					int playerWon = game->getPlayerWon(currentState);
//					//std::cout << currentState << std::endl;
//					//std::cout << "Player won: " << playerWon << " neuralNetColor: " << colorBuf << std::endl;
//					if (playerWon == colorBuf)
//						outEval.wins++;
//					else if (playerWon == 0)
//						outEval.draws++;
//					else
//						outEval.losses++;
//
//					currentStates.erase(currentStates.begin() + i);
//					i--;
//				}
//			}
//		}
//	}
//
//private:
//	int m_gamesToPlay = 0;
//	int m_currentColor = 0;
//	int m_mctsCount = 50;
//	torch::DeviceType m_device = torch::kCPU;
//};


#endif //DEEPREINFORCEMENTLEARNING_EVALUATION_H

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

// Libtorch has many warnings which clutter the output, so we ignore them
#pragma warning(push, 0)
#include <torch/torch.h>
#pragma warning(pop)

#include <AlphaZeroTraining.h>
#include <Chess/ChessAdapter.h>
#include <Chess/Engine/ChessEngine.h>
#include <MonteCarloTreeSearch.h>
#include <NeuralNetworks/DefaultNeuralNet.h>

namespace
{
	using Clock = std::chrono::steady_clock;

	double elapsedSeconds(Clock::time_point start)
	{
		return std::chrono::duration<double>(Clock::now() - start).count();
	}

	torch::DeviceType parseDevice(const std::string& device)
	{
		if (device == "cuda" || device == "gpu")
			return torch::kCUDA;
		return torch::kCPU;
	}

	std::string deviceName(torch::DeviceType device)
	{
		return device == torch::kCUDA ? "CUDA" : "CPU";
	}

	ChessAdapter& chessAdapter()
	{
		static ChessAdapter adapter;
		return adapter;
	}

	void printResult(const std::string& name, int samples, double seconds)
	{
		const double perSecond = samples / seconds;
		std::cout << std::fixed << std::setprecision(2)
			<< name << ": " << seconds << " s for " << samples << " samples -> "
			<< perSecond << " samples/s\n";
	}

	// Measures the raw neural net forward pass throughput (self play inference).
	void benchmarkNeuralNetEval(torch::DeviceType device, const std::vector<int>& batchSizes, int iterations)
	{
		std::cout << "\n=== Neural net forward (eval) on " << deviceName(device) << " ===\n";
		DefaultNeuralNet net(14, 8, 8, 4096, device);
		net.setToEval();
		torch::NoGradGuard noGrad;

		for (const int batchSize : batchSizes)
		{
			auto input = torch::randn({ batchSize, 14, 8, 8 }).to(device);
			// Warmup
			for (int i = 0; i < 3; i++)
				net.calculate(input);

			const auto start = Clock::now();
			for (int i = 0; i < iterations; i++)
				net.calculate(input);
			const double seconds = elapsedSeconds(start);

			printResult("batch " + std::to_string(batchSize), batchSize * iterations, seconds);
		}
	}

	// Measures the training step (forward + backward + optimizer) throughput.
	void benchmarkNeuralNetTrain(torch::DeviceType device, const std::vector<int>& batchSizes, int iterations)
	{
		std::cout << "\n=== Neural net training step on " << deviceName(device) << " ===\n";
		DefaultNeuralNet net(14, 8, 8, 4096, device);
		net.setToTraining();
		net.setLearningRate(0.02f);

		for (const int batchSize : batchSizes)
		{
			auto input = torch::randn({ batchSize, 14, 8, 8 }).to(device);
			auto valueTarget = torch::randn({ batchSize, 1 }).to(device);
			auto probsTarget = torch::softmax(torch::randn({ batchSize, 4096 }), 1).to(device);

			// Warmup
			for (int i = 0; i < 2; i++)
			{
				auto output = net.calculate(input);
				net.training(std::get<0>(output), std::get<1>(output), probsTarget, valueTarget);
			}

			const auto start = Clock::now();
			for (int i = 0; i < iterations; i++)
			{
				auto output = net.calculate(input);
				net.training(std::get<0>(output), std::get<1>(output), probsTarget, valueTarget);
			}
			const double seconds = elapsedSeconds(start);
		}
	}

	// Measures the neural net forward pass when issued from several host threads at once.
	// This isolates the effect of concurrent CUDA submissions from the batch size.
	void benchmarkNeuralNetEvalMultiThread(torch::DeviceType device, int batchSize, int numThreads, int iterations)
	{
		std::cout << "\n=== Neural net forward (eval) on " << deviceName(device)
			<< ", batch " << batchSize << ", " << numThreads << " host threads ===\n";
		DefaultNeuralNet net(14, 8, 8, 4096, device);
		net.setToEval();

		std::vector<std::thread> threads;
		std::mutex mutex;
		const auto start = Clock::now();

		for (int t = 0; t < numThreads; t++)
		{
			threads.emplace_back([&]()
			{
				torch::NoGradGuard noGrad;
				auto input = torch::randn({ batchSize, 14, 8, 8 }).to(device);
				for (int i = 0; i < iterations; i++)
				{
					std::scoped_lock lock(mutex);
					net.calculate(input);
				}
			});
		}

		for (auto& thread : threads)
			thread.join();

		const double seconds = elapsedSeconds(start);
		printResult("batch " + std::to_string(batchSize) + " x " + std::to_string(numThreads) + " threads"
			, batchSize * numThreads * iterations, seconds);
	}

	// Measures the pure CPU cost of chess move generation (no neural net involved).
	void benchmarkMoveGeneration(int iterations)
	{
		std::cout << "\n=== Chess move generation (CPU) ===\n";
		auto& adapter = chessAdapter();
		const std::vector<std::string> fens =
		{
			"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -",
			"r1bqkbnr/pppp1ppp/2n5/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R b KQkq -",
			"r2q1rk1/ppp2ppp/2n1bn2/3pp3/3PP3/2N1BN2/PPP2PPP/R2Q1RK1 w - -"
		};

		for (const auto& fen : fens)
		{
			const auto state = adapter.getGameStateFromString(fen, adapter.getInitialPlayer());

			// Warmup
			for (int i = 0; i < 100; i++)
				adapter.getAllPossibleMoves(state, state.currentPlayer);

			const auto start = Clock::now();
			size_t moveCount = 0;
			for (int i = 0; i < iterations; i++)
				moveCount += adapter.getAllPossibleMoves(state, state.currentPlayer).size();
			const double seconds = elapsedSeconds(start);

			std::cout << std::fixed << std::setprecision(2)
				<< "getAllPossibleMoves (" << moveCount / iterations << " moves): "
				<< seconds << " s for " << iterations << " calls -> "
				<< (iterations / seconds) << " calls/s\n";
		}
	}

	// Measures the MCTS cost with a mock expansion, which isolates the CPU bound
	// tree search / move generation from the neural net.
	void benchmarkMcts(int simulations, int games)
	{
		std::cout << "\n=== MCTS (mock expansion, no neural net) ===\n";
		auto& adapter = chessAdapter();
		DefaultNeuralNet net(14, 8, 8, 4096, torch::kCPU);

		const auto start = Clock::now();
		for (int g = 0; g < games; g++)
		{
			auto cache = MonteCarloTreeSearchCache<ChessAdapter::GameState, ChessAdapter, true>(torch::kCPU, &adapter, &net);
			auto mcts = MonteCarloTreeSearch<ChessAdapter::GameState, ChessAdapter, true>(&cache, &adapter, torch::kCPU);
			auto state = adapter.getInitialGameState();
			mcts.search(static_cast<size_t>(simulations), state, state.currentPlayer);
		}
		const double seconds = elapsedSeconds(start);

		std::cout << std::fixed << std::setprecision(2)
			<< seconds << " s for " << games << " searches of " << simulations << " simulations -> "
			<< (simulations * games / seconds) << " simulations/s\n";
	}


	// Runs real training iterations so the self play / training split can be observed.
	// selfPlayBatch <= 0 derives the batch size from the game/thread count.
	void benchmarkFullTraining(torch::DeviceType device, int iterations, int selfPlayGames, int mctsCount, int cpuThreads, int selfPlayBatch)
	{
		// Fixed seed + deterministic play (no random opening moves) so that different
		// benchmark configurations are directly comparable.
		torch::manual_seed(1234567);
		std::cout << "\n=== Full chess training on " << deviceName(device) << " ==="
			<< " games=" << selfPlayGames << " mcts=" << mctsCount
			<< " threads=" << cpuThreads
			<< " batch=" << (selfPlayBatch > 0 ? selfPlayBatch : std::max(1, selfPlayGames / cpuThreads)) << "\n";
		auto& adapter = chessAdapter();
		DefaultNeuralNet net(14, 8, 8, 4096, device);

		AlphaZeroTrainingParameters params;
		params.neuralNetPath = "NeuralNets/Training/ChessBenchmark";
		params.TRAINING_DONT_USE_DRAWS = false;
		params.RESTRICT_GAME_LENGTH = true;
		params.DRAW_AFTER_COUNT_OF_STEPS = 256;
		params.TRAINING_ITERATIONS = iterations;
		params.SELF_PLAY_MCTS_COUNT = mctsCount;
		params.NUM_SELF_PLAY_GAMES = selfPlayGames;
		params.SELFPLAY_BATCH_SIZE = selfPlayBatch > 0 ? selfPlayBatch : std::max(1, selfPlayGames / cpuThreads);
		params.NUMBER_CPU_THREADS = cpuThreads;
		params.TRAINING_BATCH_SIZE = 512;
		params.MIN_REPLAY_MEMORY_SIZE = 1;
		params.MAX_REPLAY_MEMORY_SIZE = 500000;
		params.SAVE_ITERATION_COUNT = 1000000;
		params.RANDOM_MOVE_COUNT = 20;
		params.PRINT_SECTION_TIMING = true;

		auto training = AlphaZeroTraining<ChessAdapter::GameState, ChessAdapter>(&adapter, &net, device);
		training.setTrainingParams(params);

		const auto start = Clock::now();
		training.runTraining();
		const double seconds = elapsedSeconds(start);

		std::cout << std::fixed << std::setprecision(2)
			<< "Total: " << seconds << " s for " << iterations << " iterations -> "
			<< (seconds / iterations) << " s/iteration\n";
	}

	int intArg(char** argv, int argc, int index, int fallback)
	{
		return index < argc ? std::atoi(argv[index]) : fallback;
	}
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cout << "Usage: Benchmark <mode> [options]\n"
			<< "  nneval   <cpu|cuda> [iterations]\n"
			<< "  nnevalmt <cpu|cuda> <batchSize> <threads> <iterationsPerThread>\n"
			<< "  nntrain  <cpu|cuda> [iterations]\n"
			<< "  movegen  [iterations]\n"
			<< "  mcts     <simulations> <games>\n"
			<< "  train    <cpu|cuda> <iterations> <selfPlayGames> <mctsCount> <cpuThreads> [selfPlayBatch]\n"
			<< "           (selfPlayBatch defaults to selfPlayGames / cpuThreads)\n";
		return 0;
	}

	const std::string mode = argv[1];
	const std::vector<int> batchSizes{ 1, 8, 16, 32, 64, 128, 256, 512 };

	if (mode == "nneval")
	{
		benchmarkNeuralNetEval(parseDevice(argc > 2 ? argv[2] : "cpu"), batchSizes, intArg(argv, argc, 3, 20));
	}
	else if (mode == "nntrain")
	{
		benchmarkNeuralNetTrain(parseDevice(argc > 2 ? argv[2] : "cpu"), batchSizes, intArg(argv, argc, 3, 20));
	}
	else if (mode == "nnevalmt")
	{
		// nnevalmt <cpu|cuda> <batchSize> <threads> <iterationsPerThread>
		benchmarkNeuralNetEvalMultiThread(parseDevice(argc > 2 ? argv[2] : "cpu")
			, intArg(argv, argc, 3, 25)
			, intArg(argv, argc, 4, 8)
			, intArg(argv, argc, 5, 20));
	}
	else if (mode == "movegen")
	{
		benchmarkMoveGeneration(intArg(argv, argc, 2, 20000));
	}
	else if (mode == "mcts")
	{
		benchmarkMcts(intArg(argv, argc, 2, 800), intArg(argv, argc, 3, 5));
	}
	else if (mode == "train")
	{
		benchmarkFullTraining(parseDevice(argc > 2 ? argv[2] : "cpu")
			, intArg(argv, argc, 3, 1)
			, intArg(argv, argc, 4, 40)
			, intArg(argv, argc, 5, 800)
			, intArg(argv, argc, 6, 8)
			, intArg(argv, argc, 7, 0));
	}
	else
	{
		std::cout << "Unknown mode: " << mode << std::endl;
		return 1;
	}

	return 0;
}

#include <iostream>
#include <string>
#include <chrono>
#include <omp.h>
#include <utility>
#include <NeuralNetworks/DefaultNeuralNet.h>
#include <QApplication>
#include "GameHandling/ChessHandler.h"
#include "GameHandling/ConnectFourHandler.h"
#include "GameHandling/TicTacToeHandler.h"
#include "UI/MainWindow.h"

void initRNGS()
{
	srand(time(NULL));
	std::mt19937 mt_rand(time(NULL));
}

#ifdef _WIN32
#undef main
#endif

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	MainWindow w;
	w.show();

	return app.exec();
}
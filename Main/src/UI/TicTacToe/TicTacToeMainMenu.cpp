#include "UI/TicTacToe/TicTacToeMainMenu.h"

TicTacToeMainMenu::TicTacToeMainMenu(QWidget* w, QWidget* parent) : QWidget(parent), gameMainUi(new Ui::TicTacToeMain) {
	gameMainUi->setupUi(this);
	handler = TicTacToeHandler();
	trainingMenu = new TicTacToeTraining(trainingMenu);
	miniMaxMenu = new TicTacToeMiniMaxMenu(miniMaxMenu);
	neuralNetMenu = new TicTacToeNeuralNetMenu(neuralNetMenu);
}

TicTacToeMainMenu::~TicTacToeMainMenu() {
	delete gameMainUi;
	delete trainingMenu;
	delete miniMaxMenu;
	delete neuralNetMenu;
}

void TicTacToeMainMenu::reset() {
	this->setParent(nullptr);
	trainingMenu->reset();
	miniMaxMenu->reset();
	neuralNetMenu->reset();
}

void TicTacToeMainMenu::on_MiniMaxButton_clicked() {
	this->parentWidget()->layout()->replaceWidget(this, miniMaxMenu);
	this->setParent(nullptr);
}

void TicTacToeMainMenu::on_TwoPlayerButton_clicked() {
	handler.startTwoPlayerTicTacToeGame();
}

void TicTacToeMainMenu::on_NeuralNetButton_clicked() {
	this->parentWidget()->layout()->replaceWidget(this, neuralNetMenu);
	this->setParent(nullptr);
}

void TicTacToeMainMenu::on_TrainingButton_clicked() {
	this->parentWidget()->layout()->replaceWidget(this, trainingMenu);
	this->setParent(nullptr);
}

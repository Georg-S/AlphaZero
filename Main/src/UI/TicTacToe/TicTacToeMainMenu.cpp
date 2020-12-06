#include "UI/TicTacToe/TicTacToeMainMenu.h"

TicTacToeMainMenu::TicTacToeMainMenu(QWidget* w, QWidget* parent) : QWidget(parent), gameMainUi(new Ui::TicTacToeMain) {
	gameMainUi->setupUi(this);
	handler = TicTacToeHandler();
	trainingMenu = new TicTacToeTraining(trainingMenu);
	minMaxMenu = new TicTacToeMinMaxMenu(minMaxMenu);
	neuralNetMenu = new TicTacToeNeuralNetMenu(neuralNetMenu);
}

TicTacToeMainMenu::~TicTacToeMainMenu() {
	delete gameMainUi;
	delete trainingMenu;
	delete minMaxMenu;
	delete neuralNetMenu;
}

void TicTacToeMainMenu::reset() {
	this->setParent(nullptr);
	trainingMenu->reset();
	minMaxMenu->reset();
	neuralNetMenu->reset();
}

void TicTacToeMainMenu::on_MinMaxButton_clicked() {
	this->parentWidget()->layout()->replaceWidget(this, minMaxMenu);
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

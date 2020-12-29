#include "UI/Chess/ChessMainMenu.h"

ChessMainMenu::ChessMainMenu(QWidget* w, QWidget* parent) : QWidget(parent), gameMainUi(new Ui::ChessMain)
{
	gameMainUi->setupUi(this);
	trainingMenu = new ChessTrainingMenu(trainingMenu);
	miniMaxMenu = new ChessMiniMaxMenu(miniMaxMenu);
	neuralNetMenu = new ChessNeuralNetMenu(neuralNetMenu);
}

ChessMainMenu::~ChessMainMenu()
{
	delete gameMainUi;
	delete trainingMenu;
	delete miniMaxMenu;
	delete neuralNetMenu;
}

void ChessMainMenu::reset()
{
	this->setParent(nullptr);
	trainingMenu->reset();
	miniMaxMenu->reset();
	neuralNetMenu->reset();
}

void ChessMainMenu::on_MiniMaxButton_clicked()
{
	this->parentWidget()->layout()->replaceWidget(this, miniMaxMenu);
	this->setParent(nullptr);
}

void ChessMainMenu::on_TwoPlayerButton_clicked()
{
	ChessHandler::startTwoPlayerChessGame();
}

void ChessMainMenu::on_NeuralNetButton_clicked()
{
	this->parentWidget()->layout()->replaceWidget(this, neuralNetMenu);
	this->setParent(nullptr);
}

void ChessMainMenu::on_TrainingButton_clicked()
{
	this->parentWidget()->layout()->replaceWidget(this, trainingMenu);
	this->setParent(nullptr);
}

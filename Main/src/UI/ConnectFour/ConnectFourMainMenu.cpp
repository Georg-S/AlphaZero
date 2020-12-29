#include "UI/ConnectFour/ConnectFourMainMenu.h"

ConnectFourMainMenu::ConnectFourMainMenu(QWidget* w, QWidget* parent) : QWidget(parent), gameMainUi(new Ui::ConnectFourMain)
{
	gameMainUi->setupUi(this);
	miniMaxMenu = new ConnectFourMiniMaxMenu(miniMaxMenu);
	neuralNetMenu = new ConnectFourNeuralNetMenu(neuralNetMenu);
	trainingMenu = new ConnectFourTraining(trainingMenu);
}

ConnectFourMainMenu::~ConnectFourMainMenu()
{
	delete gameMainUi;
}

void ConnectFourMainMenu::on_MiniMaxButton_clicked()
{
	this->parentWidget()->layout()->replaceWidget(this, miniMaxMenu);
	this->setParent(nullptr);
}

void ConnectFourMainMenu::reset()
{
	miniMaxMenu->reset();
	neuralNetMenu->reset();
	trainingMenu->reset();
	this->setParent(nullptr);

}

void ConnectFourMainMenu::on_TwoPlayerButton_clicked()
{
	ConnectFourHandler handler = ConnectFourHandler();
	handler.startTwoPlayerConnectFourGame();
}

void ConnectFourMainMenu::on_NeuralNetButton_clicked()
{
	this->parentWidget()->layout()->replaceWidget(this, neuralNetMenu);
	this->setParent(nullptr);
}

void ConnectFourMainMenu::on_TrainingButton_clicked()
{
	this->parentWidget()->layout()->replaceWidget(this, trainingMenu);
	this->setParent(nullptr);
}


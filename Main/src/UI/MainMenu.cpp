#include "UI/MainMenu.h"

MainMenu::MainMenu(QWidget* w, QWidget* parent) : QWidget(parent), mainMenuForm(new Ui::MainMenuForm) {
	mainMenuForm->setupUi(this);
	ticTacToeMainMenu = new TicTacToeMainMenu(ticTacToeMainMenu);
	connectFourMainMenu = new ConnectFourMainMenu(connectFourMainMenu);
	chessMainMenu = new ChessMainMenu(chessMainMenu);
}

MainMenu::~MainMenu() {
	delete mainMenuForm;
	delete connectFourMainMenu;
	delete ticTacToeMainMenu;
}

void MainMenu::on_TicTacToeButton_clicked() {
	this->parentWidget()->layout()->replaceWidget(this, ticTacToeMainMenu);
	this->setParent(nullptr);
}

void MainMenu::on_ConnectFourButton_clicked() {
	this->parentWidget()->layout()->replaceWidget(this, connectFourMainMenu);
	this->setParent(nullptr);
}

void MainMenu::on_ChessButton_clicked() {
	this->parentWidget()->layout()->replaceWidget(this, chessMainMenu);
	this->setParent(nullptr);
}

void MainMenu::reset() {
	ticTacToeMainMenu->reset();
	connectFourMainMenu->reset();
	chessMainMenu->reset();
	this->setParent(nullptr);
}

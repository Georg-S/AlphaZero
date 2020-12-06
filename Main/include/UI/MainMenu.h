#ifndef DEEPREINFORCEMENTLEARNING_MAINMENU_H
#define DEEPREINFORCEMENTLEARNING_MAINMENU_H

#include <iostream>
#include <functional>
#include <QWidget>
#include "ui_MainMenu.h"
#include "UI/ConnectFour/ConnectFourMainMenu.h"
#include "UI/Chess/ChessMainMenu.h"
#include "UI//TicTacToe/TicTacToeMainMenu.h"

class MainMenu : public QWidget {
	Q_OBJECT;

public:
	explicit MainMenu(std::function<void(bool test)> setHomeButtonVisibility, QWidget* w, QWidget* parent = nullptr);
	~MainMenu();
	void reset();

	Ui::MainMenuForm* mainMenuForm;

private slots:
	void on_ConnectFourButton_clicked();
	void on_ChessButton_clicked();
	void on_TicTacToeButton_clicked();
private:
	TicTacToeMainMenu* ticTacToeMainMenu;
	ConnectFourMainMenu* connectFourMainMenu;
	ChessMainMenu* chessMainMenu;
	std::function<void(bool test)> setHomeButtonVisibility;
};


#endif //DEEPREINFORCEMENTLEARNING_MAINMENU_H

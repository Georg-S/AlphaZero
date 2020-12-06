#ifndef DEEPREINFORCEMENTLEARNING_MAINMENU_H
#define DEEPREINFORCEMENTLEARNING_MAINMENU_H

#include <iostream>
#include <QWidget>
#include "ui_MainMenu.h"
#include "UI/ConnectFour/ConnectFourMainMenu.h"
#include "UI/Chess/ChessMainMenu.h"
#include "UI//TicTacToe/TicTacToeMainMenu.h"

class MainMenu : public QWidget {
	Q_OBJECT;

public:
	explicit MainMenu(QWidget* w, QWidget* parent = nullptr);
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
};


#endif //DEEPREINFORCEMENTLEARNING_MAINMENU_H

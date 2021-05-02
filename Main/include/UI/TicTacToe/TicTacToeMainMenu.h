#ifndef DEEPREINFORCEMENTLEARNING_TICTACTOEMAINMENU_H
#define DEEPREINFORCEMENTLEARNING_TICTACTOEMAINMENU_H

#include <QWidget>
#include "TicTacToeTraining.h"
#include "TicTacToeMiniMaxMenu.h"
#include "TicTacToeNeuralNetMenu.h"
#include "ui_TicTacToeMain.h"

// QT somehow has Problems with libtorch so this preProcessor instructions are needed
#undef slots
#include "GameHandling/TicTacToeHandler.h"
#define slots Q_SLOTS

class TicTacToeMainMenu : public QWidget
{
	Q_OBJECT;

public:
	explicit TicTacToeMainMenu(QWidget* w, QWidget* parent = nullptr);
	~TicTacToeMainMenu();
	void reset();

private slots:
	void on_MiniMaxButton_clicked();
	void on_TwoPlayerButton_clicked();
	void on_NeuralNetButton_clicked();
	void on_TrainingButton_clicked();

private:
	Ui::TicTacToeMain* gameMainUi;
	TicTacToeTraining* trainingMenu;
	TicTacToeMiniMaxMenu* miniMaxMenu;
	TicTacToeNeuralNetMenu* neuralNetMenu;
	TicTacToeHandler handler;
};


#endif //DEEPREINFORCEMENTLEARNING_TICTACTOEMAINMENU_H

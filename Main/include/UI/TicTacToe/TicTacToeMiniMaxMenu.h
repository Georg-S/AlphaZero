#ifndef DEEPREINFORCEMENTLEARNING_TICTACTOEMINIMAXMENU_H
#define DEEPREINFORCEMENTLEARNING_TICTACTOEMINIMAXMENU_H

#include <QWidget>
#include <QIntValidator>
#include "ui_MiniMax.h"

// QT somehow has Problems with libtorch so this preProcessor instructions are needed
#undef slots
#include "GameHandling/TicTacToeHandler.h"
#define slots Q_SLOTS

class TicTacToeMiniMaxMenu : public QWidget {
	Q_OBJECT;

public:
	explicit TicTacToeMiniMaxMenu(QWidget* w, QWidget* parent = nullptr);
	~TicTacToeMiniMaxMenu();

	void reset();
private slots:
	void on_PlayButton_clicked();

private:
	Ui::TwoPlayerMiniMaxWidget* ticTacToeMiniMax;
	TicTacToeHandler gameHandler;
};


#endif //DEEPREINFORCEMENTLEARNING_TICTACTOEMINIMAXMENU_H

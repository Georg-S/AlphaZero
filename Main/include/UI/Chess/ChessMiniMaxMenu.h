#ifndef DEEPREINFORCEMENTLEARNING_CHESSMINIMAXMENU_H
#define DEEPREINFORCEMENTLEARNING_CHESSMINIMAXMENU_H

#include <QWidget>
#include <QIntValidator>
#include "ui_MiniMax.h"

#undef slots
#include "GameHandling/ChessHandler.h"
#define slots Q_SLOTS

class ChessMiniMaxMenu : public QWidget {
	Q_OBJECT;
public:
	explicit ChessMiniMaxMenu(QWidget* w, QWidget* parent = nullptr);
	~ChessMiniMaxMenu();

	void reset();

private slots:
	void on_PlayButton_clicked();

private:
	Ui::TwoPlayerMiniMaxWidget* chessMiniMax;
	ChessHandler gameHandler;
};


#endif //DEEPREINFORCEMENTLEARNING_CHESSMINIMAXMENU_H

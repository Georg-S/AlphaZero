#ifndef DEEPREINFORCEMENTLEARNING_CONNECTFOURMINIMAXMENU_H
#define DEEPREINFORCEMENTLEARNING_CONNECTFOURMINIMAXMENU_H

#include <QWidget>
#include <QIntValidator>
#include "ui_MiniMax.h"

// QT somehow has Problems with libtorch so this preProcessor instructions are needed
#undef slots
#include "GameHandling/ConnectFourHandler.h"
#define slots Q_SLOTS


class ConnectFourMiniMaxMenu : public QWidget
{
	Q_OBJECT;

public:
	explicit ConnectFourMiniMaxMenu(QWidget* w, QWidget* parent = nullptr);
	~ConnectFourMiniMaxMenu();
	void reset();

private slots:
	void on_PlayButton_clicked();

private:
	Ui::TwoPlayerMiniMaxWidget* connectFourMiniMax;
	ConnectFourHandler gameHandler;

};


#endif //DEEPREINFORCEMENTLEARNING_CONNECTFOURMINIMAXMENU_H

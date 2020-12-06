#ifndef DEEPREINFORCEMENTLEARNING_TICTACTOEMINMAXMENU_H
#define DEEPREINFORCEMENTLEARNING_TICTACTOEMINMAXMENU_H

#include <QWidget>
#include <QIntValidator>
#include "ui_MinMax.h"

// QT somehow has Problems with libtorch so this preProcessor instructions are needed
#undef slots
#include "GameHandling/TicTacToeHandler.h"
#define slots Q_SLOTS

class TicTacToeMinMaxMenu : public QWidget {
    Q_OBJECT;

public:
    explicit TicTacToeMinMaxMenu(QWidget* w, QWidget* parent = nullptr);
    ~TicTacToeMinMaxMenu();

    void reset();
private slots:
    void on_PlayButton_clicked();

private:
    Ui::TwoPlayerMinMaxWidget* ticTacToeMinMax;
    TicTacToeHandler gameHandler;
};


#endif //DEEPREINFORCEMENTLEARNING_TICTACTOEMINMAXMENU_H

#ifndef DEEPREINFORCEMENTLEARNING_CONNECTFOURMINMAXMENU_H
#define DEEPREINFORCEMENTLEARNING_CONNECTFOURMINMAXMENU_H

#include <QWidget>
#include <QIntValidator>
#include "ui_MinMax.h"

// QT somehow has Problems with libtorch so this preProcessor instructions are needed
#undef slots
#include "GameHandling/ConnectFourHandler.h"
#define slots Q_SLOTS


class ConnectFourMinMaxMenu : public QWidget{
Q_OBJECT;

public:
    explicit ConnectFourMinMaxMenu(QWidget* w, QWidget* parent = nullptr);
    ~ConnectFourMinMaxMenu();

    void reset();

private slots:
    void on_PlayButton_clicked();

private:
    Ui::TwoPlayerMinMaxWidget* connectFourMinMax;
    ConnectFourHandler gameHandler;

};


#endif //DEEPREINFORCEMENTLEARNING_CONNECTFOURMINMAXMENU_H

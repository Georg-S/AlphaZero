#ifndef PROTOTYPE_CHESSMINMAXMENU_H
#define PROTOTYPE_CHESSMINMAXMENU_H

#include <QWidget>
#include <QIntValidator>
#include "ui_MinMax.h"

#undef slots
#include "GameHandling/ChessHandler.h"
#define slots Q_SLOTS

class ChessMinMaxMenu : public QWidget {
    Q_OBJECT;
public:
    explicit ChessMinMaxMenu(QWidget* w, QWidget* parent = nullptr);
    ~ChessMinMaxMenu();

    void reset();

private slots:
    void on_PlayButton_clicked();

private:
    Ui::TwoPlayerMinMaxWidget* chessMinMax;
    ChessHandler gameHandler;
};


#endif //PROTOTYPE_CHESSMINMAXMENU_H

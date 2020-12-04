#ifndef PROTOTYPE_CONNECTFOURMAINMENU_H
#define PROTOTYPE_CONNECTFOURMAINMENU_H

#include <QWidget>
#include "ConnectFourMinMaxMenu.h"
#include "ConnectFourNeuralNetMenu.h"
#include "ConnectFourTraining.h"
#include "ui_ConnectFourMain.h"

// QT somehow has Problems with libtorch so this preProcessor instructions are needed
#undef slots
#include "GameHandling/ConnectFourHandler.h"
#define slots Q_SLOTS

class ConnectFourMainMenu : public QWidget {
    Q_OBJECT;

public:
    explicit ConnectFourMainMenu(QWidget* w, QWidget* parent = nullptr);
    ~ConnectFourMainMenu();

    void reset();

private slots:
    void on_MinMaxButton_clicked();
    void on_TwoPlayerButton_clicked();
    void on_NeuralNetButton_clicked();
    void on_TrainingButton_clicked();



private:
    Ui::ConnectFourMain* gameMainUi;
    ConnectFourMinMaxMenu* minMaxMenu;
    ConnectFourNeuralNetMenu* neuralNetMenu;
    ConnectFourTraining* trainingMenu;
};


#endif //PROTOTYPE_CONNECTFOURMAINMENU_H

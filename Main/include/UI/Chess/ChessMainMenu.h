#ifndef DEEPREINFORCEMENTLEARNING_CHESSMAINMENU_H
#define DEEPREINFORCEMENTLEARNING_CHESSMAINMENU_H

#include <QWidget>
#include "ChessTrainingMenu.h"
#include "ChessMinMaxMenu.h"
#include "ChessNeuralNetMenu.h"
#include "ui_ChessMain.h"

#undef slots
#include "GameHandling/ChessHandler.h"
#define slots Q_SLOTS

class ChessMainMenu : public QWidget{
    Q_OBJECT;

public:
    explicit ChessMainMenu(QWidget* w, QWidget* parent = nullptr);
    ~ChessMainMenu();

    void reset();

private slots:
    void on_MinMaxButton_clicked();
    void on_TwoPlayerButton_clicked();
    void on_NeuralNetButton_clicked();
    void on_TrainingButton_clicked();

private:
    Ui::ChessMain* gameMainUi;
    ChessTrainingMenu* trainingMenu;
    ChessMinMaxMenu* minMaxMenu;
    ChessNeuralNetMenu* neuralNetMenu;
};


#endif //DEEPREINFORCEMENTLEARNING_CHESSMAINMENU_H

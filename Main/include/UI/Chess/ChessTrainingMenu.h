#ifndef PROTOTYPE_CHESSTRAININGMENU_H
#define PROTOTYPE_CHESSTRAININGMENU_H

#include <QWidget>
#include <QIntValidator>
#include "ui_Training.h"

#undef slots
#include "GameHandling/ChessHandler.h"
#define slots Q_SLOTS

class ChessTrainingMenu : public QWidget{
    Q_OBJECT;
public:
    explicit ChessTrainingMenu(QWidget* w, QWidget* parent = nullptr);
    ~ChessTrainingMenu();

    void reset();

private slots:
    void on_RestrictGameLengthNoRadio_clicked();
    void on_RestrictGameLengthYesRadio_clicked();
    void on_StartTrainingButton_clicked();

private:
    void initDefaultValues();
    void initInputValidators();
    void runTraining();
    TrainingParameters getParametersFromInput();

    Ui::TrainingWidget* trainingUi;
    ChessHandler gameHandler;
};


#endif //PROTOTYPE_CHESSTRAININGMENU_H

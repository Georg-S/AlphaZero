#ifndef DEEPREINFORCEMENTLEARNING_TICTACTOETRAINING_H
#define DEEPREINFORCEMENTLEARNING_TICTACTOETRAINING_H

#include <QWidget>
#include <QIntValidator>
#include "ui_Training.h"

// QT somehow has Problems with libtorch so this preProcessor instructions are needed
#undef slots
#include "GameHandling/TicTacToeHandler.h"
#define slots Q_SLOTS

class TicTacToeTraining : public QWidget {
    Q_OBJECT;
public:
    explicit TicTacToeTraining(QWidget* w, QWidget* parent = nullptr);
    ~TicTacToeTraining();

    void reset();
private slots:
    void on_RestrictGameLengthNoRadio_clicked();
    void on_RestrictGameLengthYesRadio_clicked();
    void on_StartTrainingButton_clicked();

private:
    void initDefaultValues();
    void initInputValidators();
    TrainingParameters getParametersFromInput();


    Ui::TrainingWidget* trainingUi;
    TicTacToeHandler gameHandler;
};


#endif //DEEPREINFORCEMENTLEARNING_TICTACTOETRAINING_H

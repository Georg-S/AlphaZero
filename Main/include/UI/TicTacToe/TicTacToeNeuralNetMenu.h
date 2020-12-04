#ifndef PROTOTYPE_TICTACTOENEURALNETMENU_H
#define PROTOTYPE_TICTACTOENEURALNETMENU_H

#include <QWidget>
#include <QIntValidator>
#include "ui_NeuralNet.h"

// QT somehow has Problems with libtorch so this preProcessor instructions are needed
#undef slots
#include "GameHandling/TicTacToeHandler.h"
#define slots Q_SLOTS

class TicTacToeNeuralNetMenu : public QWidget {
    Q_OBJECT;

public:
    TicTacToeNeuralNetMenu(QWidget* w, QWidget* parent = nullptr);
    ~TicTacToeNeuralNetMenu();

    void reset();
private slots:
    void on_PlayButton_clicked();


private:
    Ui::NeuralNetWidget* neuralNetUi;
    TicTacToeHandler gameHandler;
};


#endif //PROTOTYPE_TICTACTOENEURALNETMENU_H

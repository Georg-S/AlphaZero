#include "UI/TicTacToe/TicTacToeMinMaxMenu.h"

TicTacToeMinMaxMenu::TicTacToeMinMaxMenu(QWidget* w, QWidget* parent) : QWidget(parent) , ticTacToeMinMax(new Ui::TwoPlayerMinMaxWidget){
    ticTacToeMinMax->setupUi(this);
    ticTacToeMinMax->MinMaxDepthLabel->setVisible(false);
    ticTacToeMinMax->MinMaxDepthInput->setVisible(false);
    ticTacToeMinMax->PlayerColor1Radio->setText("Cross");
    ticTacToeMinMax->PlayerColor2Radio->setText("Dot");
    gameHandler = TicTacToeHandler();
}

TicTacToeMinMaxMenu::~TicTacToeMinMaxMenu() {
    delete ticTacToeMinMax;
}

void TicTacToeMinMaxMenu::on_PlayButton_clicked() {
    int playerColor = 2;
    if(ticTacToeMinMax->PlayerColor1Radio->isChecked())
        playerColor = 1;

    gameHandler.ticTacToeAgainstMinMaxAi(playerColor);
}

void TicTacToeMinMaxMenu::reset() {
    this->setParent(nullptr);
}


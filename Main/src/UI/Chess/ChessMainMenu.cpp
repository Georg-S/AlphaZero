#include "UI/Chess/ChessMainMenu.h"

ChessMainMenu::ChessMainMenu(QWidget* w, QWidget* parent) : QWidget(parent), gameMainUi(new Ui::ChessMain) {
    gameMainUi->setupUi(this);
    trainingMenu = new ChessTrainingMenu(trainingMenu);
    minMaxMenu = new ChessMinMaxMenu(minMaxMenu);
    neuralNetMenu = new ChessNeuralNetMenu(neuralNetMenu);
}

ChessMainMenu::~ChessMainMenu() {
    delete gameMainUi;
    delete trainingMenu;
    delete minMaxMenu;
    delete neuralNetMenu;
}

void ChessMainMenu::reset() {
    this->setParent(nullptr);
    trainingMenu->reset();
    minMaxMenu->reset();
    neuralNetMenu->reset();
}

void ChessMainMenu::on_MinMaxButton_clicked() {
    this->parentWidget()->layout()->replaceWidget(this,minMaxMenu);
    this->setParent(nullptr);
}

void ChessMainMenu::on_TwoPlayerButton_clicked() {
    ChessHandler::startTwoPlayerChessGame();
}

void ChessMainMenu::on_NeuralNetButton_clicked() {
    this->parentWidget()->layout()->replaceWidget(this, neuralNetMenu);
    this->setParent(nullptr);
}

void ChessMainMenu::on_TrainingButton_clicked() {
    this->parentWidget()->layout()->replaceWidget(this,trainingMenu);
    this->setParent(nullptr);
}

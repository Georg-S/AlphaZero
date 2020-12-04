#include "UI/ConnectFour/ConnectFourMinMaxMenu.h"

ConnectFourMinMaxMenu::ConnectFourMinMaxMenu(QWidget* w, QWidget* parent) : QWidget(parent), connectFourMinMax(new Ui::TwoPlayerMinMaxWidget){
    connectFourMinMax->setupUi(this);
    connectFourMinMax->MinMaxDepthInput->setValidator( new QIntValidator(0, INT32_MAX, this));
    connectFourMinMax->PlayerColor1Radio->setText("Yellow");
    connectFourMinMax->PlayerColor2Radio->setText("Red");
    gameHandler = ConnectFourHandler();
}

ConnectFourMinMaxMenu::~ConnectFourMinMaxMenu() {
    delete connectFourMinMax;
}

void ConnectFourMinMaxMenu::reset() {
    this->setParent(nullptr);
}

void ConnectFourMinMaxMenu::on_PlayButton_clicked() {
    int minMaxDepth = connectFourMinMax->MinMaxDepthInput->text().toInt();
    cn4::PlayerColor playerColor;
    if(connectFourMinMax->PlayerColor1Radio->isChecked())
        playerColor = cn4::YELLOW;
    else
        playerColor = cn4::RED;

    gameHandler.connectFourAgainstMinMaxAi(minMaxDepth, playerColor);
}

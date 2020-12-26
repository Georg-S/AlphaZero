#include "UI/ConnectFour/ConnectFourMiniMaxMenu.h"

ConnectFourMiniMaxMenu::ConnectFourMiniMaxMenu(QWidget* w, QWidget* parent) : QWidget(parent), connectFourMiniMax(new Ui::TwoPlayerMiniMaxWidget) {
	connectFourMiniMax->setupUi(this);
	connectFourMiniMax->MiniMaxDepthInput->setValidator(new QIntValidator(0, INT32_MAX, this));
	connectFourMiniMax->PlayerColor1Radio->setText("Yellow");
	connectFourMiniMax->PlayerColor2Radio->setText("Red");
	gameHandler = ConnectFourHandler();
}

ConnectFourMiniMaxMenu::~ConnectFourMiniMaxMenu() {
	delete connectFourMiniMax;
}

void ConnectFourMiniMaxMenu::reset() {
	this->setParent(nullptr);
}

void ConnectFourMiniMaxMenu::on_PlayButton_clicked() {
	int miniMaxDepth = connectFourMiniMax->MiniMaxDepthInput->text().toInt();
	cn4::PlayerColor playerColor;
	if (connectFourMiniMax->PlayerColor1Radio->isChecked())
		playerColor = cn4::PlayerColor::YELLOW;
	else
		playerColor = cn4::PlayerColor::RED;

	gameHandler.connectFourAgainstMiniMaxAi(miniMaxDepth, playerColor);
}

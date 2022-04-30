#include "UI/TicTacToe/TicTacToeMiniMaxMenu.h"

TicTacToeMiniMaxMenu::TicTacToeMiniMaxMenu(QWidget* w, QWidget* parent)
	: QWidget(parent), ticTacToeMiniMax(new Ui::TwoPlayerMiniMaxWidget)
{
	ticTacToeMiniMax->setupUi(this);
	ticTacToeMiniMax->MiniMaxDepthLabel->setVisible(false);
	ticTacToeMiniMax->MiniMaxDepthInput->setVisible(false);
	ticTacToeMiniMax->PlayerColor1Radio->setText("Cross");
	ticTacToeMiniMax->PlayerColor2Radio->setText("Dot");
	gameHandler = TicTacToeHandler();
}

TicTacToeMiniMaxMenu::~TicTacToeMiniMaxMenu()
{
	delete ticTacToeMiniMax;
}

void TicTacToeMiniMaxMenu::on_PlayButton_clicked()
{
	ttt::PlayerColor playerColor = ttt::PlayerColor::DOT;
	if (ticTacToeMiniMax->PlayerColor1Radio->isChecked())
		playerColor = ttt::PlayerColor::CROSS;

	gameHandler.ticTacToeAgainstMiniMaxAi(playerColor);
}

void TicTacToeMiniMaxMenu::reset()
{
	this->setParent(nullptr);
}


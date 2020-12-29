#include "UI/Chess/ChessMiniMaxMenu.h"

ChessMiniMaxMenu::ChessMiniMaxMenu(QWidget* w, QWidget* parent) : QWidget(parent), chessMiniMax(new Ui::TwoPlayerMiniMaxWidget) 
{
	chessMiniMax->setupUi(this);
	chessMiniMax->MiniMaxDepthInput->setText("3");
	chessMiniMax->MiniMaxDepthInput->setValidator(new QIntValidator(0, INT32_MAX, this));
	chessMiniMax->PlayerColor1Radio->setText("White");
	chessMiniMax->PlayerColor2Radio->setText("Black");
	gameHandler = ChessHandler();
}

ChessMiniMaxMenu::~ChessMiniMaxMenu() 
{
	delete chessMiniMax;
}

void ChessMiniMaxMenu::reset()
{
	this->setParent(nullptr);
}

void ChessMiniMaxMenu::on_PlayButton_clicked()
{
	int miniMaxDepth = chessMiniMax->MiniMaxDepthInput->text().toInt();
	chess::PieceColor playerColor;
	if (chessMiniMax->PlayerColor1Radio->isChecked())
		playerColor = chess::PieceColor::WHITE;
	else
		playerColor = chess::PieceColor::BLACK;

	gameHandler.chessAgainstMiniMaxAi(miniMaxDepth, playerColor);
}

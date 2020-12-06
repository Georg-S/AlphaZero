#include "UI/Chess/ChessMinMaxMenu.h"

ChessMinMaxMenu::ChessMinMaxMenu(QWidget* w, QWidget* parent) : QWidget(parent), chessMinMax(new Ui::TwoPlayerMinMaxWidget) {
	chessMinMax->setupUi(this);
	chessMinMax->MinMaxDepthInput->setText("3");
	chessMinMax->MinMaxDepthInput->setValidator(new QIntValidator(0, INT32_MAX, this));
	chessMinMax->PlayerColor1Radio->setText("White");
	chessMinMax->PlayerColor2Radio->setText("Black");
	gameHandler = ChessHandler();
}

ChessMinMaxMenu::~ChessMinMaxMenu() {
	delete chessMinMax;
}

void ChessMinMaxMenu::reset() {
	this->setParent(nullptr);
}

void ChessMinMaxMenu::on_PlayButton_clicked() {
	int minMaxDepth = chessMinMax->MinMaxDepthInput->text().toInt();
	chess::PieceColor playerColor;
	if (chessMinMax->PlayerColor1Radio->isChecked())
		playerColor = chess::PieceColor::WHITE;
	else
		playerColor = chess::PieceColor::BLACK;

	gameHandler.chessAgainstMinMaxAi(minMaxDepth, playerColor);
}

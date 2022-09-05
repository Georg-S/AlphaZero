#include "UI/Chess/ChessNeuralNetMenu.h"

ChessNeuralNetMenu::ChessNeuralNetMenu(QWidget* w, QWidget* parent) : QWidget(parent), neuralNetUi(new Ui::NeuralNetWidget)
{
	neuralNetUi->setupUi(this);
	gameHandler = ChessHandler();
	neuralNetUi->MCTSCountInput->setValidator(new QIntValidator(0, INT32_MAX, this));
	neuralNetUi->PlayerColor1Radio->setText("White");
	neuralNetUi->PlayerColor2Radio->setText("Black");
}

ChessNeuralNetMenu::~ChessNeuralNetMenu()
{
	delete neuralNetUi;
}

void ChessNeuralNetMenu::reset()
{
	this->setParent(nullptr);
}

void ChessNeuralNetMenu::on_PlayButton_clicked()
{
	ceg::PieceColor playerColor;
	if (neuralNetUi->PlayerColor1Radio->isChecked())
		playerColor = ceg::PieceColor::WHITE;
	else
		playerColor = ceg::PieceColor::BLACK;

	torch::DeviceType device;
	if (neuralNetUi->CPURadio->isChecked())
		device = torch::kCPU;
	else
		device = torch::kCUDA;

	bool randomize;
	if (neuralNetUi->RandomizeYesRadio->isChecked())
		randomize = true;
	else
		randomize = false;

	int mctsCount = neuralNetUi->MCTSCountInput->text().toInt();
	std::string netName = neuralNetUi->NetNameInput->text().toStdString();

	gameHandler.chessAgainstNeuralNetAi(playerColor, netName, mctsCount, randomize, device);
}

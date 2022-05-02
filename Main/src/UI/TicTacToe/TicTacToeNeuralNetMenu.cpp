#include "UI/TicTacToe/TicTacToeNeuralNetMenu.h"

TicTacToeNeuralNetMenu::TicTacToeNeuralNetMenu(QWidget* w, QWidget* parent) : QWidget(parent), neuralNetUi(new Ui::NeuralNetWidget)
{
	neuralNetUi->setupUi(this);
	gameHandler = TicTacToeHandler();
	neuralNetUi->MCTSCountInput->setValidator(new QIntValidator(0, INT32_MAX, this));
	neuralNetUi->MCTSCountInput->setText("50");
	neuralNetUi->PlayerColor1Radio->setText("Cross");
	neuralNetUi->PlayerColor2Radio->setText("Dot");
}

TicTacToeNeuralNetMenu::~TicTacToeNeuralNetMenu()
{
	delete neuralNetUi;
}

void TicTacToeNeuralNetMenu::reset()
{
	this->setParent(nullptr);
}

void TicTacToeNeuralNetMenu::on_PlayButton_clicked()
{
	ttt::PlayerColor playerColor = ttt::PlayerColor::DOT;
	if (neuralNetUi->PlayerColor1Radio->isChecked())
		playerColor = ttt::PlayerColor::CROSS;

	torch::DeviceType device = torch::kCUDA;
	if (neuralNetUi->CPURadio->isChecked())
		device = torch::kCPU;

	bool randomize = false;
	if (neuralNetUi->RandomizeYesRadio->isChecked())
		randomize = true;

	int mctsCount = neuralNetUi->MCTSCountInput->text().toInt();
	std::string netName = neuralNetUi->NetNameInput->text().toStdString();

	gameHandler.ticTacToeAgainstNeuralNetAi(playerColor, netName, mctsCount, randomize, device);
}


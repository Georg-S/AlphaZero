#include "UI/ConnectFour/ConnectFourNeuralNetMenu.h"

ConnectFourNeuralNetMenu::ConnectFourNeuralNetMenu(QWidget* w, QWidget* parent)
	: QWidget(parent), neuralNetUi(new Ui::NeuralNetWidget)
{
	neuralNetUi->setupUi(this);
	gameHandler = ConnectFourHandler();
	neuralNetUi->MCTSCountInput->setValidator(new QIntValidator(0, INT32_MAX, this));
	neuralNetUi->PlayerColor1Radio->setText("Yellow");
	neuralNetUi->PlayerColor2Radio->setText("Red");
}

ConnectFourNeuralNetMenu::~ConnectFourNeuralNetMenu()
{
	delete neuralNetUi;
}

void ConnectFourNeuralNetMenu::reset()
{
	this->setParent(nullptr);
}

void ConnectFourNeuralNetMenu::on_PlayButton_clicked()
{
	cn4::PlayerColor playerColor;
	if (neuralNetUi->PlayerColor1Radio->isChecked())
		playerColor = cn4::PlayerColor::YELLOW;
	else
		playerColor = cn4::PlayerColor::RED;

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

	gameHandler.connectFourAgainstNeuralNetAi(playerColor, netName, mctsCount, randomize, device);
}

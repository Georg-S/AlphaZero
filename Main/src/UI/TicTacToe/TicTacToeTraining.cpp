#include "UI/TicTacToe/TicTacToeTraining.h"

TicTacToeTraining::TicTacToeTraining(QWidget* w, QWidget* parent) : QWidget(parent), trainingUi(new Ui::TrainingWidget)
{
	trainingUi->setupUi(this);
	gameHandler = TicTacToeHandler();
	initDefaultValues();
	initInputValidators();
}

TicTacToeTraining::~TicTacToeTraining()
{
	delete trainingUi;
}

void TicTacToeTraining::initDefaultValues()
{
	trainingUi->ReplayMemorySizeInput->setText("100000");
	trainingUi->UseDrawsYesRadio->setChecked(true);
	trainingUi->RestrictGameLengthNoRadio->setChecked(false);
	trainingUi->MaxGameLengthInput->setText("0");
	trainingUi->TrainingIterationsInput->setText("99999");
	trainingUi->SelfPlayMCTSCountInput->setText("50");
	trainingUi->SelfPlayGamesCountInput->setText("1000");
	trainingUi->TrainingBatchSizeInput->setText("100");
	trainingUi->SaveIterationCountInput->setText("1");
	trainingUi->RandomizedMoveCountInput->setText("3");
	trainingUi->LearningRateInput->setText("0.001");
	trainingUi->FilterCountInput->setText("512");
	trainingUi->GPURadio->setChecked(true);
	trainingUi->CpuThreadsInput->setText("1");
}

void TicTacToeTraining::initInputValidators()
{
	trainingUi->MaxGameLengthInput->setValidator(new QIntValidator(0, INT32_MAX, this));
	trainingUi->ReplayMemorySizeInput->setValidator(new QIntValidator(0, INT32_MAX, this));
	trainingUi->FilterCountInput->setValidator(new QIntValidator(0, INT32_MAX, this));
	trainingUi->SelfPlayMCTSCountInput->setValidator(new QIntValidator(0, INT32_MAX, this));
	trainingUi->TrainingBatchSizeInput->setValidator(new QIntValidator(0, INT32_MAX, this));
	trainingUi->SaveIterationCountInput->setValidator(new QIntValidator(0, INT32_MAX, this));
	trainingUi->SelfPlayGamesCountInput->setValidator(new QIntValidator(0, INT32_MAX, this));
	trainingUi->TrainingIterationsInput->setValidator(new QIntValidator(0, INT32_MAX, this));
	trainingUi->RandomizedMoveCountInput->setValidator(new QIntValidator(0, INT32_MAX, this));
	trainingUi->LearningRateInput->setValidator(new QDoubleValidator(this));
	trainingUi->CpuThreadsInput->setValidator(new QIntValidator(0, INT32_MAX, this));
}

TrainingParameters TicTacToeTraining::getParametersFromInput()
{
	TrainingParameters params;

	torch::DeviceType device = torch::kCPU;
	if (trainingUi->GPURadio->isChecked())
		device = torch::kCUDA;

	params.useDraws = trainingUi->UseDrawsYesRadio->isChecked();
	params.restrictGameLength = trainingUi->RestrictGameLengthYesRadio->isChecked();
	if (!params.restrictGameLength)
		params.maxGameLength = 0;
	else
		params.maxGameLength = trainingUi->MaxGameLengthInput->text().toInt();
	params.replayMemorySize = trainingUi->ReplayMemorySizeInput->text().toInt();
	params.trainingIterations = trainingUi->TrainingIterationsInput->text().toInt();
	params.selfPlayMctsCount = trainingUi->SelfPlayMCTSCountInput->text().toInt();
	params.selfPlayGamesCount = trainingUi->SelfPlayGamesCountInput->text().toInt();
	params.trainingBatchSize = trainingUi->TrainingBatchSizeInput->text().toInt();
	params.saveIterationCount = trainingUi->SaveIterationCountInput->text().toInt();
	params.randomizedMoveCount = trainingUi->RandomizedMoveCountInput->text().toInt();
	params.learningRate = trainingUi->LearningRateInput->text().toFloat();
	params.filterCount = trainingUi->FilterCountInput->text().toInt();
	params.device = device;
	params.cpuThreads = trainingUi->CpuThreadsInput->text().toInt();

	return params;
}

void TicTacToeTraining::on_RestrictGameLengthNoRadio_clicked()
{
	trainingUi->MaxGameLengthLabel->setEnabled(false);
	trainingUi->MaxGameLengthInput->setEnabled(false);
}

void TicTacToeTraining::on_RestrictGameLengthYesRadio_clicked()
{
	trainingUi->MaxGameLengthInput->setEnabled(true);
	trainingUi->MaxGameLengthLabel->setEnabled(true);
}

void TicTacToeTraining::on_StartTrainingButton_clicked()
{
	if (!trainingRunning)
	{
		trainingRunning = true;
		trainingUi->StartTrainingButton->setText("Training is running");
		trainingUi->StartTrainingButton->setEnabled(false);

		TrainingParameters params = getParametersFromInput();
		auto trainingFunc = [=]() {gameHandler.runTraining(params); };
		trainingThread = new TrainingThread(trainingFunc);
		connect(trainingThread, &TrainingThread::trainingFinished, this, &TicTacToeTraining::handleTrainingFinished);

		trainingThread->start();
	}
}

void TicTacToeTraining::handleTrainingFinished()
{
	trainingRunning = false;
	trainingUi->StartTrainingButton->setText("Start Training");
	trainingUi->StartTrainingButton->setEnabled(true);
	delete trainingThread;
}

void TicTacToeTraining::reset()
{
	this->setParent(nullptr);
}

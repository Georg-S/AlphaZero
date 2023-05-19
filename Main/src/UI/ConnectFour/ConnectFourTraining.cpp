#include "UI/ConnectFour/ConnectFourTraining.h"

ConnectFourTraining::ConnectFourTraining(QWidget* w, QWidget* parent) : QWidget(parent), trainingUi(new Ui::TrainingWidget)
{
	trainingUi->setupUi(this);
	initInputValidators();
	initDefaultValues();
	gameHandler = ConnectFourHandler();
}

ConnectFourTraining::~ConnectFourTraining()
{
	delete trainingUi;
}

void ConnectFourTraining::reset()
{
	this->setParent(nullptr);
}

void ConnectFourTraining::on_RestrictGameLengthNoRadio_clicked()
{
	trainingUi->MaxGameLengthLabel->setEnabled(false);
	trainingUi->MaxGameLengthInput->setEnabled(false);
}

void ConnectFourTraining::on_RestrictGameLengthYesRadio_clicked()
{
	trainingUi->MaxGameLengthInput->setEnabled(true);
	trainingUi->MaxGameLengthLabel->setEnabled(true);
}

void ConnectFourTraining::on_StartTrainingButton_clicked()
{
	trainingUi->StartTrainingButton->setEnabled(false);
	trainingUi->StartTrainingButton->setText("Training is running");

	runTraining();
}

void ConnectFourTraining::runTraining()
{
	if (!trainingRunning)
	{
		trainingRunning = true;
		trainingUi->StartTrainingButton->setText("Training is running");
		trainingUi->StartTrainingButton->setEnabled(false);

		TrainingParameters params = getParametersFromInput();
		auto trainingFunc = [=]() {gameHandler.runTraining(params); };
		trainingThread = new TrainingThread(trainingFunc);
		connect(trainingThread, &TrainingThread::trainingFinished, this, &ConnectFourTraining::handleTrainingFinished);

		trainingThread->start();
	}
}

void ConnectFourTraining::handleTrainingFinished()
{
	trainingRunning = false;
	trainingUi->StartTrainingButton->setText("Start Training");
	trainingUi->StartTrainingButton->setEnabled(true);
	delete trainingThread;
}

void ConnectFourTraining::initDefaultValues()
{
	trainingUi->ReplayMemorySizeInput->setText("200000");
	trainingUi->UseDrawsYesRadio->setChecked(true);
	trainingUi->RestrictGameLengthNoRadio->setChecked(true);
	trainingUi->MaxGameLengthInput->setText("0");
	trainingUi->TrainingIterationsInput->setText("40");
	trainingUi->SelfPlayMCTSCountInput->setText("100");
	trainingUi->SelfPlayBatchSizeInput->setText("100");
	trainingUi->SelfPlayGamesCountInput->setText("1000");
	trainingUi->TrainingBatchSizeInput->setText("100");
	trainingUi->SaveIterationCountInput->setText("1");
	trainingUi->RandomizedMoveCountInput->setText("10");
	trainingUi->LearningRateInput->setText("0.01");
	trainingUi->ContinueTrainingNoRadio->setChecked(true);
	trainingUi->GPURadio->setChecked(true);
	trainingUi->CpuThreadsInput->setText("1");
}

void ConnectFourTraining::initInputValidators()
{
	trainingUi->MaxGameLengthInput->setValidator(new QIntValidator(0, INT32_MAX, this));
	trainingUi->ReplayMemorySizeInput->setValidator(new QIntValidator(0, INT32_MAX, this));
	trainingUi->SelfPlayMCTSCountInput->setValidator(new QIntValidator(0, INT32_MAX, this));
	trainingUi->SelfPlayBatchSizeInput->setValidator(new QIntValidator(0, INT32_MAX, this));
	trainingUi->TrainingBatchSizeInput->setValidator(new QIntValidator(0, INT32_MAX, this));
	trainingUi->SaveIterationCountInput->setValidator(new QIntValidator(0, INT32_MAX, this));
	trainingUi->SelfPlayGamesCountInput->setValidator(new QIntValidator(0, INT32_MAX, this));
	trainingUi->TrainingIterationsInput->setValidator(new QIntValidator(0, INT32_MAX, this));
	trainingUi->RandomizedMoveCountInput->setValidator(new QIntValidator(0, INT32_MAX, this));
	trainingUi->LearningRateInput->setValidator(new QDoubleValidator(this));
	trainingUi->CpuThreadsInput->setValidator(new QIntValidator(0, INT32_MAX, this));
}

TrainingParameters ConnectFourTraining::getParametersFromInput()
{
	TrainingParameters params = {};

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
	params.selfPlayBatchSize = trainingUi->SelfPlayBatchSizeInput->text().toInt();
	params.selfPlayGamesCount = trainingUi->SelfPlayGamesCountInput->text().toInt();
	params.trainingBatchSize = trainingUi->TrainingBatchSizeInput->text().toInt();
	params.saveIterationCount = trainingUi->SaveIterationCountInput->text().toInt();
	params.randomizedMoveCount = trainingUi->RandomizedMoveCountInput->text().toInt();
	params.learningRate = trainingUi->LearningRateInput->text().toFloat();
	params.continueTraining = trainingUi->ContinueTrainingYesRadio->isChecked();
	params.device = device;
	params.cpuThreads = trainingUi->CpuThreadsInput->text().toInt();

	return params;
}

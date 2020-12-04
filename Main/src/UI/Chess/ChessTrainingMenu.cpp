#include "UI/Chess/ChessTrainingMenu.h"

ChessTrainingMenu::ChessTrainingMenu(QWidget* w, QWidget* parent) : QWidget(parent), trainingUi(new Ui::TrainingWidget) {
    trainingUi->setupUi(this);
    gameHandler = ChessHandler();
    initDefaultValues();
    initInputValidators();
}

ChessTrainingMenu::~ChessTrainingMenu() {
 delete trainingUi;
}

void ChessTrainingMenu::on_RestrictGameLengthNoRadio_clicked() {
    trainingUi->MaxGameLengthLabel->setEnabled(false);
    trainingUi->MaxGameLengthInput->setEnabled(false);
}

void ChessTrainingMenu::on_RestrictGameLengthYesRadio_clicked() {
    trainingUi->MaxGameLengthInput->setEnabled(true);
    trainingUi->MaxGameLengthLabel->setEnabled(true);
}

void ChessTrainingMenu::on_StartTrainingButton_clicked() {
    trainingUi->StartTrainingButton->setEnabled(false);
    trainingUi->StartTrainingButton->setText("Training is running");

    runTraining();
}

void ChessTrainingMenu::initDefaultValues() {
    trainingUi->ReplayMemorySizeInput->setText("200000");
    trainingUi->UseDrawsYesRadio->setChecked(true);
    trainingUi->RestrictGameLengthYesRadio->setChecked(true);
    trainingUi->MaxGameLengthInput->setText("80");
    trainingUi->TrainingIterationsInput->setText("9999");
    trainingUi->SelfPlayMCTSCountInput->setText("200");
    trainingUi->SelfPlayGamesCountInput->setText("250");
    trainingUi->TrainingBatchSizeInput->setText("100");
    trainingUi->SaveIterationCountInput->setText("1");
    trainingUi->RandomizedMoveCountInput->setText("20");
    trainingUi->LearningRateInput->setText("0.001");
    trainingUi->FilterCountInput->setText("512");
    trainingUi->GPURadio->setChecked(true);
    trainingUi->MaxGameLengthInput->setEnabled(true);
    trainingUi->MaxGameLengthLabel->setEnabled(true);
    trainingUi->CpuThreadsInput->setText("1");
}

void ChessTrainingMenu::initInputValidators() {
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

void ChessTrainingMenu::runTraining() {
    TrainingParameters params = getParametersFromInput();
    gameHandler.runTraining(params);
}

TrainingParameters ChessTrainingMenu::getParametersFromInput() {
    TrainingParameters params;

    torch::DeviceType device = torch::kCPU;
    if(trainingUi->GPURadio->isChecked())
        device = torch::kCUDA;

    params.useDraws = trainingUi->UseDrawsYesRadio->isChecked();
    params.restrictGameLength = trainingUi->RestrictGameLengthYesRadio->isChecked();
    if(!params.restrictGameLength)
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

void ChessTrainingMenu::reset() {
    this->setParent(nullptr);
}

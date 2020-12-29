#include "UI/TrainingThread.h"

TrainingThread::TrainingThread(std::function<void()> runTraining)
{
	this->runTraining = runTraining;
	this->setTerminationEnabled(true);
}

void TrainingThread::run()
{
	this->runTraining();
	emit trainingFinished();
}

void TrainingThread::setToExecuteFunction(std::function<void()> runTraining)
{
	this->runTraining = runTraining;
}

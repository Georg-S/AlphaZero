#ifndef DEEPREINFORCEMENTLEARNING_TRAININGTHREAD_H
#define DEEPREINFORCEMENTLEARNING_TRAININGTHREAD_H

#include <qthread.h>
#include <functional>

class TrainingThread : public QThread {
	Q_OBJECT
public:
	TrainingThread(std::function<void()> runTraining = nullptr);
	void run() override;
	void setToExecuteFunction(std::function<void()> runTraining);

signals:
	void trainingFinished();
private:
	std::function<void()> runTraining;
};


#endif //DEEPREINFORCEMENTLEARNING_TRAININGTHREAD_H
#ifndef DEEPREINFORCEMENTLEARNING_CONNECTFOURTRAINING_H
#define DEEPREINFORCEMENTLEARNING_CONNECTFOURTRAINING_H

#include <iostream>
#include <QWidget>
#include <QIntValidator>
#include "ui_Training.h"
#include "UI/TrainingThread.h"
// QT somehow has Problems with libtorch so this preProcessor instructions are needed
#undef slots
#include "GameHandling/ConnectFourHandler.h"
#define slots Q_SLOTS


class ConnectFourTraining : public QWidget
{
	Q_OBJECT;

public:
	explicit ConnectFourTraining(QWidget* w, QWidget* parent = nullptr);
	~ConnectFourTraining();
	void reset();

private slots:
	void on_RestrictGameLengthNoRadio_clicked();
	void on_RestrictGameLengthYesRadio_clicked();
	void on_StartTrainingButton_clicked();

private:
	void handleTrainingFinished();
	void initDefaultValues();
	void initInputValidators();
	TrainingParameters getParametersFromInput();
	void runTraining();

	Ui::TrainingWidget* trainingUi;
	ConnectFourHandler gameHandler;
	TrainingThread* trainingThread = nullptr;
	bool trainingRunning = false;
};


#endif //DEEPREINFORCEMENTLEARNING_CONNECTFOURTRAINING_H

#ifndef DEEPREINFORCEMENTLEARNING_CHESSTRAININGMENU_H
#define DEEPREINFORCEMENTLEARNING_CHESSTRAININGMENU_H

#include <QWidget>
#include <QIntValidator>
#include "ui_Training.h"
#include "UI/TrainingThread.h"

#undef slots
#include "GameHandling/ChessHandler.h"
#define slots Q_SLOTS

class ChessTrainingMenu : public QWidget {
	Q_OBJECT;
public:
	explicit ChessTrainingMenu(QWidget* w, QWidget* parent = nullptr);
	~ChessTrainingMenu();
	void reset();

private slots:
	void on_RestrictGameLengthNoRadio_clicked();
	void on_RestrictGameLengthYesRadio_clicked();
	void on_StartTrainingButton_clicked();

private:
	void initDefaultValues();
	void initInputValidators();
	void runTraining();
	TrainingParameters getParametersFromInput();
	void handleTrainingFinished();

	Ui::TrainingWidget* trainingUi;
	ChessHandler gameHandler;
	TrainingThread* trainingThread = nullptr;
	bool trainingRunning = false;
};


#endif //DEEPREINFORCEMENTLEARNING_CHESSTRAININGMENU_H

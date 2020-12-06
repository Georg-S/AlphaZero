#ifndef DEEPREINFORCEMENTLEARNING_CONNECTFOURNEURALNETMENU_H
#define DEEPREINFORCEMENTLEARNING_CONNECTFOURNEURALNETMENU_H

#include <QWidget>
#include <QIntValidator>
#include "ui_NeuralNet.h"

// QT somehow has Problems with libtorch so this preProcessor instructions are needed
#undef slots
#include "GameHandling/ConnectFourHandler.h"
#define slots Q_SLOTS

class ConnectFourNeuralNetMenu : public QWidget {
	Q_OBJECT;


public:
	explicit ConnectFourNeuralNetMenu(QWidget* w, QWidget* parent = nullptr);
	~ConnectFourNeuralNetMenu();

	void reset();

private slots:
	void on_PlayButton_clicked();

private:
	Ui::NeuralNetWidget* neuralNetUi;
	ConnectFourHandler gameHandler;
};


#endif //DEEPREINFORCEMENTLEARNING_CONNECTFOURNEURALNETMENU_H

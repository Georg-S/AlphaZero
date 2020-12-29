#ifndef DEEPREINFORCEMENTLEARNING_CHESSNEURALNETMENU_H
#define DEEPREINFORCEMENTLEARNING_CHESSNEURALNETMENU_H

#include <QWidget>
#include <QIntValidator>
#include "ui_NeuralNet.h"

// QT somehow has Problems with libtorch so this preProcessor instructions are needed
#undef slots
#include "GameHandling/ChessHandler.h"
#define slots Q_SLOTS

class ChessNeuralNetMenu : public QWidget
{
	Q_OBJECT;
public:
	explicit ChessNeuralNetMenu(QWidget* w, QWidget* parent = nullptr);
	~ChessNeuralNetMenu();
	void reset();

private slots:
	void on_PlayButton_clicked();

private:
	Ui::NeuralNetWidget* neuralNetUi;
	ChessHandler gameHandler;
};


#endif //DEEPREINFORCEMENTLEARNING_CHESSNEURALNETMENU_H

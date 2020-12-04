#ifndef PROTOTYPE_MAINWINDOW_H
#define PROTOTYPE_MAINWINDOW_H

#include <iostream>
#include <QtWidgets/QMainWindow>
#include <qlabel.h>
#include <qdebug.h>
#include <string>
#include <math.h>
#include "ui_MainWindow.h"
#include "MainMenu.h"


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
    void on_HomeButton_clicked();
private:
    void setup();

	Ui::MainWindow *ui;
	MainMenu* mainMenu;
};

#endif //PROTOTYPE_MAINWINDOW_H
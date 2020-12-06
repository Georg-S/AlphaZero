#include "UI/MainWindow.h"


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	setup();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::setup() {
	ui->setupUi(this);
	mainMenu = new MainMenu(mainMenu);
	ui->verticalLayout->addWidget(mainMenu);
}

void MainWindow::on_HomeButton_clicked() {
	mainMenu->reset();
	ui->verticalLayout->addWidget(mainMenu);
}

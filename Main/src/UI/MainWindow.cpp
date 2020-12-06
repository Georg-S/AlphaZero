#include "UI/MainWindow.h"


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	setup();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::setup() 
{
	ui->setupUi(this);
	auto setHomeButtonVisibility = [&](bool value) {ui->HomeButton->setVisible(value); };
	setHomeButtonVisibility(false);
	mainMenu = new MainMenu(setHomeButtonVisibility, mainMenu);
	ui->verticalLayout->addWidget(mainMenu);
}

void MainWindow::on_HomeButton_clicked() 
{
	mainMenu->reset();
	ui->verticalLayout->addWidget(mainMenu);
	ui->HomeButton->setVisible(false);
}
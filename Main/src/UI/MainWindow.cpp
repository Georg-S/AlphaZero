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
	initialSize = this->size();
}

void MainWindow::hardResize(const QSize& size)
{
	setFixedSize(size);
	setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
	setMinimumSize(0, 0);
}

void MainWindow::on_HomeButton_clicked()
{
	mainMenu->reset();
	ui->verticalLayout->addWidget(mainMenu);
	ui->HomeButton->setVisible(false);
	hardResize(this->initialSize);
}
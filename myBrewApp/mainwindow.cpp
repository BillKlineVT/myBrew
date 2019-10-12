#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myBrew.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	myBrew * myBrewInstance = new myBrew(this);

	setCentralWidget(myBrewInstance);
}

MainWindow::~MainWindow()
{
    delete ui;
}

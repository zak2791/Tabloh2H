#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    lblStatus = new QLabel();
    statusBar()->addWidget(lblStatus);
    winSettings = ui->winSettings;
    closeProg = ui->close;

}

MainWindow::~MainWindow()
{
    delete ui;
}

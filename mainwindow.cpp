#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QCloseEvent>

#include <QDebug>
#include <QFileDialog>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    lblStatus = new QLabel();
    statusBar()->addWidget(lblStatus);
    winSettings = ui->winSettings;
    closeProg = ui->close;
    time = ui->setTime;

    QAction* openFile = ui->openFile;
    connect(openFile, SIGNAL(triggered()), this, SLOT(openFile()));

    QSettings settings("settings.ini",QSettings::IniFormat);
    settings.beginGroup("files");
    lastDir = settings.value("lastDir", "").toString();
    listFiles = settings.value("listFiles", "").toString().split(";");
    settings.endGroup();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent* e){
    int ret = QMessageBox::question(this, tr("Выход"),
                                    tr("Вы уверены?"),
                                    QMessageBox::Yes | QMessageBox::No
                                    );
    if(ret == QMessageBox::Yes)
        qApp->exit(0);
    else
        e->ignore();
}

void MainWindow::openFile(){
    qDebug()<<lastDir;
    QString file = QFileDialog::getOpenFileName(this, tr("Выберите файл"),  lastDir, tr("*.xlsx *.xls"));
    if(file == "")
        return;
    qDebug()<<lastDir<<listFiles<<file<<file.lastIndexOf('/')<<file.remove(file.lastIndexOf('/'), 100);
    QSettings settings;
    settings.beginGroup("files");
    settings.setValue("lastDir", file.remove(file.lastIndexOf('/'), 100));
    //listFiles = settings.value("listFiles", "").toString().split(";");
    settings.endGroup();
    //file.lastIndexOf('/');
}

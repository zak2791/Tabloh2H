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
    //closeProg = ui->close;
    time = ui->setTime;

    fileSportsmens = "";

    QAction* openFile = ui->openFile;
    connect(openFile, SIGNAL(triggered()), this, SLOT(openFile()));

    QSettings settings("settings.ini",QSettings::IniFormat);
    settings.beginGroup("files");
    lastDir = settings.value("lastDir", "").toString();
    QString lFiles = settings.value("listFiles", "").toString();
    settings.endGroup();

    lastFiles = ui->mFile->addMenu("Последние файлы");
    closeProg = ui->mFile->addAction("Выход");
    if(lFiles == "")
        lastFiles->setEnabled(false);
    else{
        listFiles = lFiles.split(";");
    }


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
    fileSportsmens = file;
    listFiles.append(fileSportsmens);
    QAction* act = lastFiles->addAction(fileSportsmens);
    lastFiles->setEnabled(true);
    lastDir = file.remove(file.lastIndexOf('/'), 100);

    QSettings settings("settings.ini",QSettings::IniFormat);
    settings.beginGroup("files");
    settings.setValue("lastDir", lastDir);
    settings.setValue("listFiles", listFiles.join(";"));
    settings.endGroup();   
    settings.sync();


}

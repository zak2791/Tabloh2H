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
    actSbros = ui->actSbros;
    video = ui->video;
    no_video = ui->no_video;

#ifdef APP_LAUNCH_FROM_IDE
    fileSettings = "settings.ini";
#else
    fileSettings = "bin/settings.ini";
#endif


    fileSportsmens = "";

    QAction* openFile = ui->openFile;
    connect(openFile, SIGNAL(triggered()), this, SLOT(openFile()));

    QSettings settings(fileSettings, QSettings::IniFormat);
    settings.beginGroup("files");
    lastDir = settings.value("lastDir", "").toString();
    QString lFiles = settings.value("listFiles", "").toString();


    lastFiles = ui->mFile->addMenu("Последние файлы");
    closeProg = ui->mFile->addAction("Выход");
    if(lFiles == "")
        lastFiles->setEnabled(false);
    else{
        listFiles = lFiles.split(";");
        foreach(QString str, listFiles){
            if(QFile::exists(str)){
                QAction* act = lastFiles->addAction(str);
                act->setCheckable(true);
                if(str == listFiles.at(0)){
                    act->setChecked(true);
                    fileSportsmens = str;
                }
                connect(act, SIGNAL(triggered()), SLOT(choiceFile()));
            }
            else{
                listFiles.removeOne(str);
            }
        }
        settings.setValue("listFiles", listFiles.join(";"));
    }
    settings.endGroup();

    no_video->setCheckable(true);
    no_video->setObjectName("0");
    video->setCheckable(true);
    video->setObjectName("1");

    connect(no_video, SIGNAL(triggered()), this, SLOT(Variant()));
    connect(video,    SIGNAL(triggered()), this, SLOT(Variant()));

    no_video->setChecked(true);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Variant(){
    if(sender()->objectName() == "0"){
        if(no_video->isChecked() == true){
            video->setChecked(false);
            emit variant(0);
        }
        else
            no_video->setChecked(true);
    }
    else{
        if(video->isChecked() == true){
            no_video->setChecked(false);
            emit variant(1);
        }
        else
            video->setChecked(true);
    }

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
    QString file = QFileDialog::getOpenFileName(this, tr("Выберите файл"),  lastDir, tr("*.xlsx *.xls"));
    if(file == "")
        return;
    fileSportsmens = file;

    listFiles.removeOne(fileSportsmens);
    listFiles.prepend(fileSportsmens);

    foreach(QAction* a, lastFiles->findChildren<QAction*>())
        lastFiles->removeAction(a);

    foreach(QString str, listFiles){
        QAction* act = lastFiles->addAction(str);
        act->setCheckable(true);
        if(str == listFiles.at(0))
            act->setChecked(true);
        connect(act, SIGNAL(triggered()), SLOT(choiceFile()));
    }

    lastFiles->setEnabled(true);
    lastDir = file.remove(file.lastIndexOf('/'), 100);

    QSettings settings(fileSettings, QSettings::IniFormat);
    settings.beginGroup("files");
    settings.setValue("lastDir", lastDir);
    settings.setValue("listFiles", listFiles.join(";"));
    settings.endGroup();   
    settings.sync();

    emit newFile();
}

void MainWindow::choiceFile(){
    QAction* action = static_cast<QAction*>(sender());
    qDebug()<<action->text();
    action->setChecked(true);
    fileSportsmens = action->text();

    listFiles.removeOne(fileSportsmens);
    listFiles.prepend(fileSportsmens);

    foreach(QAction* a, lastFiles->findChildren<QAction*>())
        lastFiles->removeAction(a);

    foreach(QString str, listFiles){
        QAction* act = lastFiles->addAction(str);
        act->setCheckable(true);
        if(str == listFiles.at(0))
            act->setChecked(true);
        connect(act, SIGNAL(triggered()), SLOT(choiceFile()));
    }

    QSettings settings(fileSettings, QSettings::IniFormat);
    settings.beginGroup("files");
    settings.setValue("listFiles", listFiles.join(";"));
    settings.endGroup();
    settings.sync();

    emit newFile();
}

QString MainWindow::getFileSportsmens(void){
    return fileSportsmens;
}

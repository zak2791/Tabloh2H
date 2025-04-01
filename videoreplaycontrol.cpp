#include "videoreplaycontrol.h"
#include "qdatetime.h"
#include "qdebug.h"
#include "qsettings.h"
#include "ui_videoreplaycontrol.h"

VideoReplayControl::VideoReplayControl(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VideoReplayControl)
{
    ui->setupUi(this);
    setAutoFillBackground(true);

    connect(ui->cbCam1, &QCheckBox::clicked, this, &VideoReplayControl::turnCam1);
    connect(ui->cbCam2, &QCheckBox::clicked, this, &VideoReplayControl::turnCam2);
    connect(ui->cbCam3, &QCheckBox::clicked, this, &VideoReplayControl::turnCam3);

    procReadCam1.setProgram("cmd");
    connect(&procReadCam1, &QProcess::readyReadStandardOutput, [this](){
        QByteArray ba = procReadCam1.readAllStandardOutput();
        QImage image(100, 50, QImage:: Format_RGB666);
        if (!image.loadFromData(ba, "PNG"))
            qDebug()<<"Not loaded";
        ui->lblCam1->setPixmap(QPixmap::fromImage(image));});
    connect(&procReadCam1, &QProcess::readyReadStandardError, [this](){
        qDebug()<<procReadCam1.readAllStandardError();
        if(procRecord.state() == QProcess::Running)
            procRecord.write("q");
        ui->cbCam1->setChecked(false);
    });

    procReadCam2.setProgram("cmd");
    connect(&procReadCam2, &QProcess::readyReadStandardOutput, [this](){
        QByteArray ba = procReadCam2.readAllStandardOutput();
        QImage image(100, 50, QImage:: Format_RGB666);
        if (!image.loadFromData(ba, "PNG"))
            qDebug()<<"Not loaded";
        ui->lblCam2->setPixmap(QPixmap::fromImage(image));});
    connect(&procReadCam2, &QProcess::readyReadStandardError, [this](){
        qDebug()<<procReadCam2.readAllStandardError();
        if(procRecord.state() == QProcess::Running)
            procRecord.write("q");
        ui->cbCam2->setChecked(false);
    });

    procReadCam3.setProgram("cmd");
    connect(&procReadCam3, &QProcess::readyReadStandardOutput, [this](){
        QByteArray ba = procReadCam3.readAllStandardOutput();
        QImage image(100, 50, QImage:: Format_RGB666);
        if (!image.loadFromData(ba, "PNG"))
            qDebug()<<"Not loaded";
        ui->lblCam3->setPixmap(QPixmap::fromImage(image));});
    connect(&procReadCam3, &QProcess::readyReadStandardError, [this](){
        qDebug()<<procReadCam3.readAllStandardError();
        if(procRecord.state() == QProcess::Running)
            procRecord.write("q");
        ui->cbCam3->setChecked(false);
    });

    procRecord.setProgram("cmd");
    connect(&procRecord, &QProcess::readyReadStandardError, [this](){
        qDebug()<<procRecord.readAllStandardError();
        //ui->cbCam1->setChecked(false);

    });

    connect(ui->btnPlay, &QPushButton::clicked, [this](){
        startRecord();
    });

    connect(ui->btnPlayLast, &QPushButton::clicked, [this](){
        stopRecord();
    });

    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.beginGroup("URL");
    urlCam2 = settings.value("cam1", "").toString();
    urlCam3 = settings.value("cam2", "").toString();
    settings.endGroup();
    qDebug()<<"cam 2 = "<<urlCam2;
}

VideoReplayControl::~VideoReplayControl()
{
    delete ui;
}

void VideoReplayControl::turnCam1(bool check)
{
    if(check){
        if(procReadCam1.state() == QProcess::NotRunning){
            QStringList args("/c");
            QString url;
            if(urlSound != "")
                url = "video=" + urlCam1 + ":audio=" + urlSound;
            args<<"ffmpeg"<<"-loglevel"<<"error"
                 <<"-f"<<"dshow"<<"-rtbufsize"<<"2000M"<<"-framerate"<<fps
                 <<"-video_size"<<resolution<<"-i"<<url
                 //<<"-vcodec"<<"copy"
                 <<"-b:v"<<"1M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5001"
                 <<"-filter_complex"<<"[0:v]scale=100:50, fps=1 [vout]"
                 <<"-map"<<"[vout]"<<"-vcodec"<<"png"<<"-f"<<"image2pipe"<<"-";
            procReadCam1.setArguments(args);
            procReadCam1.start();
        }
    }
    else{
        if(procRecord.state() == QProcess::Running)
            procRecord.write("q");
        if(procReadCam1.state() == QProcess::Running)
            procReadCam1.write("q");
    }
}

void VideoReplayControl::turnCam2(bool check)
{
    // if(urlCam2 == ""){
    //     ui->cbCam2->setChecked(false);
    //     return;
    // }
    if(check){
        if(procReadCam2.state() == QProcess::NotRunning){
            QStringList args("/c");
            args<<"ffmpeg"<<"-loglevel"<<"error"
                 <<"-i"<<urlCam2
                 <<"-vcodec"<<"copy"
                 <<"-f"<<"mpegts"<<"udp://127.0.0.1:5002"
                 <<"-filter_complex"<<"[0:v]scale=100:50, fps=1 [vout]"
                 <<"-map"<<"[vout]"<<"-vcodec"<<"png"<<"-f"<<"image2pipe"<<"-";
            qDebug()<<"args 2 = "<<args;
            procReadCam2.setArguments(args);
            procReadCam2.start();
        }
    }
    else{
        if(procRecord.state() == QProcess::Running)
            procRecord.write("q");
        if(procReadCam2.state() == QProcess::Running)
            procReadCam2.write("q");
    }
}

void VideoReplayControl::turnCam3(bool check)
{
    if(check){
        if(procReadCam3.state() == QProcess::NotRunning){
            QStringList args("/c");
            args<<"ffmpeg"<<"-loglevel"<<"error"
                 <<"-i"<<urlCam3
                 <<"-vcodec"<<"copy"
                 <<"-f"<<"mpegts"<<"udp://127.0.0.1:5003"
                 <<"-filter_complex"<<"[0:v]scale=100:50, fps=1 [vout]"
                 <<"-map"<<"[vout]"<<"-vcodec"<<"png"<<"-f"<<"image2pipe"<<"-";
            procReadCam3.setArguments(args);
            procReadCam3.start();
        }
    }
    else{
        if(procRecord.state() == QProcess::Running)
            procRecord.write("q");
        if(procReadCam3.state() == QProcess::Running)
            procReadCam3.write("q");
    }
}

void VideoReplayControl::setWebCam(QString cams)
{
    urlCam1 = cams.split(";").at(0);
    fps = cams.split(";").at(1).split(" ").at(2);
    resolution = cams.split(";").at(1).split(" ").at(6);
}

void VideoReplayControl::setSound(QString sound)
{
    urlSound = sound;
}

void VideoReplayControl::setCam2(QString cam)
{
    urlCam2 = cam;
}

void VideoReplayControl::setCam3(QString cam)
{
    urlCam3 = cam;
}

void VideoReplayControl::startRecord(QString s)
{
    qDebug()<<"start0";
    QString file = "videos/" + s + "_" + QTime::currentTime().toString("hh:mm:ss");
    file.replace(":", "_");

    if(procRecord.state() == QProcess::NotRunning){
        qDebug()<<"start1";
        bool stateProc1 = procReadCam1.state() == QProcess::Running ? true : false;
        bool stateProc2 = procReadCam2.state() == QProcess::Running ? true : false;
        bool stateProc3 = procReadCam3.state() == QProcess::Running ? true : false;
        if(!(stateProc1 || stateProc2 || stateProc3))
            return;
        int countCams = (stateProc1 ? 1 : 0) + (stateProc2 ? 1 : 0) + (stateProc3 ? 1 : 0);
        QStringList args("/c");
        args<<"ffmpeg"<<"-loglevel"<<"error";
        if(countCams == 3){
            args<<"-i"<<"udp://127.0.0.1:5001";
            args<<"-i"<<"udp://127.0.0.1:5002";
            args<<"-i"<<"udp://127.0.0.1:5003";
        }
        else{
            if(stateProc1)
                args<<"-i"<<"udp://127.0.0.1:5001";
            if(stateProc2)
                args<<"-i"<<"udp://127.0.0.1:5002";
            if(stateProc3)
                args<<"-i"<<"udp://127.0.0.1:5003";
        }

        if(countCams == 3){
            args<<"-map"<<"0"<<"-map"<<"1:v"<<"-map"<<"2:v";
        }
        else if(countCams == 2){
            args<<"-map"<<"0"<<"-map"<<"1:v";
        }
        args<<"-vcodec"<<"copy";
        args<<file + ".mp4";
        qDebug()<<"args"<<args;
        procRecord.setArguments(args);
        procRecord.start();
    }
}

void VideoReplayControl::stopRecord()
{
    if(procRecord.state() == QProcess::Running){
        qDebug()<<"rec start stop";
        procRecord.write("q");
        procRecord.waitForFinished();
        if(procRecord.state() == QProcess::NotRunning)
            qDebug()<<"rec stopped";
    }
}

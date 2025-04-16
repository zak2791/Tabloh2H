  #include "videoreplaycontrol.h"
#include "qdatetime.h"
#include "qdebug.h"
#include "qdir.h"
#include "qmessagebox.h"
#include "qsettings.h"
#include <QFile>
#include "qtextcodec.h"
#include "ui_videoreplaycontrol.h"
#include <QFileDialog>

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
    connect(&procReadCam1, &QProcess::readyReadStandardOutput, this, [this](){
        QByteArray ba = procReadCam1.readAllStandardOutput();
        QImage image(100, 50, QImage:: Format_RGB666);
        if (!image.loadFromData(ba, "PNG"))
            qDebug()<<"Not loaded";
        ui->lblCam1->setPixmap(QPixmap::fromImage(image));});
    connect(&procReadCam1, &QProcess::readyReadStandardError, this, [this](){
        QByteArray ba = procReadCam1.readAllStandardError();
        QFile file("camera1.txt");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        file.write(ba, ba.length());
        file.close();
        //QTextCodec *codec = QTextCodec::codecForName("CP866");
        //QString s  = codec->toUnicode(ba);
        //qDebug()<<"s = "<<s<<QString(ba);
        QMessageBox msgbox;
        msgbox.setText(QString(ba));
        msgbox.exec();
        if(procRecord.state() == QProcess::Running)
            procRecord.write("q");
        ui->cbCam1->setChecked(false);
    });

    procReadCam2.setProgram("cmd");
    connect(&procReadCam2, &QProcess::readyReadStandardOutput, this, [this](){
        QByteArray ba = procReadCam2.readAllStandardOutput();
        QImage image(100, 50, QImage:: Format_RGB666);
        if (!image.loadFromData(ba, "PNG"))
            qDebug()<<"Not loaded";
        ui->lblCam2->setPixmap(QPixmap::fromImage(image));});
    connect(&procReadCam2, &QProcess::readyReadStandardError, this, [this](){
        QByteArray ba = procReadCam2.readAllStandardError();
        QFile file("camera2.txt");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        file.write(ba, ba.length());
        file.close();
        QMessageBox msgbox;
        msgbox.setText(QString(ba));
        msgbox.exec();
        if(procRecord.state() == QProcess::Running)
            procRecord.write("q");
        ui->cbCam2->setChecked(false);
    });

    procReadCam3.setProgram("cmd");
    connect(&procReadCam3, &QProcess::readyReadStandardOutput, this, [this](){
        QByteArray ba = procReadCam3.readAllStandardOutput();
        QImage image(100, 50, QImage:: Format_RGB666);
        if (!image.loadFromData(ba, "PNG"))
            qDebug()<<"Not loaded";
        ui->lblCam3->setPixmap(QPixmap::fromImage(image));});
    connect(&procReadCam3, &QProcess::readyReadStandardError, this, [this](){
        QByteArray ba = procReadCam3.readAllStandardError();
        QFile file("camera3.txt");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        file.write(ba, ba.length());
        file.close();
        QMessageBox msgbox;
        msgbox.setText(QString(ba));
        msgbox.exec();
        if(procRecord.state() == QProcess::Running)
            procRecord.write("q");
        ui->cbCam3->setChecked(false);
    });

    procRecord.setProgram("cmd");
    connect(&procRecord, &QProcess::readyReadStandardError, this, [this](){
        QByteArray s = procRecord.readAllStandardError();
        QFile file("record.txt");
        if (!file.open(QIODevice::Append | QIODevice::Text))
            return;
        file.write(s, s.length());
        file.close();


    });
    connect(&procRecord, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus){
                qDebug()<<"exitCode = "<<exitCode<<"exitStatus = "<<exitStatus;
                ui->label->setStatusRec(false);
            });
    connect(&procRecord, &QProcess::started, this, [=](){
        ui->label->setStatusRec(true);
    });

    connect(ui->btnPlay, &QPushButton::clicked, this, [this](){
        stopRecord();
        QString file = QFileDialog::getOpenFileName();
        if(file == "" || !file.endsWith(".mp4"))
            return;
        slowMotionPlayer = new PlayerViewer(file);
        connect(slowMotionPlayer, &PlayerViewer::sigClose, this, [=](){slowMotionPlayer->deleteLater();});
        connect(slowMotionPlayer, &PlayerViewer::sigClose, this, &VideoReplayControl::sigHidePlayer);
        connect(slowMotionPlayer, &PlayerViewer::sigImage, this, &VideoReplayControl::sigImage);

    });

    connect(ui->btnPlayLast, &QPushButton::clicked, this, [this](){
        stopRecord();
        QDir dir("videos");
        QStringList dirList = dir.entryList(QDir::Files, QDir::Time);
        if(dirList.count() == 0)
            return;
        slowMotionPlayer = new PlayerViewer("videos/" + dirList.at(0));
        connect(slowMotionPlayer, &PlayerViewer::sigClose, this, [=](){slowMotionPlayer->deleteLater();});
        connect(slowMotionPlayer, &PlayerViewer::sigClose, this, &VideoReplayControl::sigHidePlayer);
        connect(slowMotionPlayer, &PlayerViewer::sigImage, this, &VideoReplayControl::sigImage);

        emit sigShowPlayer();

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
    qDebug()<<"~VideoReplayControl()";
    if(procRecord.state() == QProcess::Running){
        procRecord.write("q");
        procRecord.waitForFinished();
    }
    if(procReadCam1.state() == QProcess::Running){
        procReadCam1.write("q");
        procReadCam1.waitForFinished();
    }
    if(procReadCam2.state() == QProcess::Running){
        procReadCam2.write("q");
        procReadCam2.waitForFinished();
    }
    if(procReadCam3.state() == QProcess::Running){
        procReadCam3.write("q");
        procReadCam3.waitForFinished();
    }

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
                 <<"-video_size"<<resolution<<"-i"<<url;

            if(streamToVk && camToVk == 1){
                args<<"-f"<<"gdigrab"<<"-framerate"<<"1"<<"-i"<<"title=TabloOnTv";
                args<<"-filter_complex"<<"[1]scale=iw/4:ih/4 [pip]; "
                        "[0][pip] overlay=main_w-overlay_w-10:main_h-overlay_h-10[out_vk]; "
                        "[0:v]scale=100:50, fps=1 [vout]; "
                        "[out_vk] drawtext=text='DEMO':x=(w / 2-text_w / 2):y=(h / 2-text_h / 2)"
                        ":fontfile=arial.ttf:fontsize=240:fontcolor=red";
                args<<"-b:v"<<"2M"<<"-f"<<"flv"<<urlVk + keyVk;
            }
            else{
                args<<"-filter_complex"<<"[0:v]scale=100:50, fps=1 [vout]";
            }

            args<<"-map"<<"[vout]"<<"-vcodec"<<"png"<<"-f"<<"image2pipe"<<"-";
            args<<"-map"<<"0"<<"-b:v"<<"4M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5001";
            procReadCam1.setArguments(args);
            procReadCam1.start();
            qDebug()<<args;
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
    if(check){
        if(procReadCam2.state() == QProcess::NotRunning){
            QStringList args("/c");
            args<<"ffmpeg"<<"-loglevel"<<"error";
            if(urlCam2.startsWith("rtsp"))
                args<<"-rtsp_transport"<<"tcp";
            args<<"-i"<<urlCam2;
            if(streamToVk && camToVk == 2){
                args<<"-f"<<"gdigrab"<<"-framerate"<<"1"<<"-i"<<"title=TabloOnTv";
                args<<"-filter_complex"<<"[1]scale=iw/4:ih/4 [pip]; "
                        "[0][pip] overlay=main_w-overlay_w-10:main_h-overlay_h-10[out_vk]; "
                        "[0:v]scale=100:50, fps=1 [vout]; "
                        "[out_vk] drawtext=text='DEMO':x=(w / 2-text_w / 2):y=(h / 2-text_h / 2)"
                        ":fontfile=arial.ttf:fontsize=240:fontcolor=red";
                args<<"-b:v"<<"2M"<<"-f"<<"flv"<<urlVk + keyVk;
            }
            else{
                args<<"-filter_complex"<<"[0:v]scale=100:50, fps=1 [vout]";
            }
            args<<"-map"<<"[vout]"<<"-vcodec"<<"png"<<"-f"<<"image2pipe"<<"-";
            args<<"-map"<<"0"<<"-vcodec"<<"copy"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5002";
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
            args<<"ffmpeg"<<"-loglevel"<<"error";
            if(urlCam3.startsWith("rtsp"))
                args<<"-rtsp_transport"<<"tcp";
            args<<"-i"<<urlCam3;
            if(streamToVk && camToVk == 3){
                args<<"-f"<<"gdigrab"<<"-framerate"<<"1"<<"-i"<<"title=TabloOnTv";
                args<<"-filter_complex"<<"[1]scale=iw/4:ih/4 [pip]; "
                        "[0][pip] overlay=main_w-overlay_w-10:main_h-overlay_h-10[out_vk]; "
                        "[0:v]scale=100:50, fps=1 [vout]; "
                        "[out_vk] drawtext=text='DEMO':x=(w / 2-text_w / 2):y=(h / 2-text_h / 2)"
                        ":fontfile=arial.ttf:fontsize=240:fontcolor=red";
                args<<"-b:v"<<"2M"<<"-f"<<"flv"<<urlVk + keyVk;
            }
            else{
                args<<"-filter_complex"<<"[0:v]scale=100:50, fps=1 [vout]";
            }
            args<<"-map"<<"[vout]"<<"-vcodec"<<"png"<<"-f"<<"image2pipe"<<"-";
            args<<"-map"<<"0"<<"-vcodec"<<"copy"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5003";
            qDebug()<<"args 2 = "<<args;
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

void VideoReplayControl::setWebCam(QString cam)
{
    urlCam1 = cam;
}

void VideoReplayControl::setParamWebCam(QList<int> param)
{
    fps = QString::number(param.at(0));
    resolution = QString::number(param.at(1)) + "x" + QString::number(param.at(2));
    qDebug()<<"fps = "<<fps<<"resolution = "<<resolution;
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

void VideoReplayControl::startRecord(bool b, QString s)
{
    ui->btnPlay->setEnabled(!b);
    ui->btnPlayLast->setEnabled(!b);
    QString file = "videos/" + s + "_" + QTime::currentTime().toString("hh:mm:ss");
    file.replace(":", "_");
    if(procRecord.state() == QProcess::NotRunning){
        bool stateProc1 = procReadCam1.state() == QProcess::Running ? true : false;
        bool stateProc2 = procReadCam2.state() == QProcess::Running ? true : false;
        bool stateProc3 = procReadCam3.state() == QProcess::Running ? true : false;
        if(!(stateProc1 || stateProc2 || stateProc3))
            return;

        QStringList args("/c");
        args<<"ffmpeg"<<"-loglevel"<<"error";

        if(stateProc1)
            args<<"-i"<<"udp://127.0.0.1:5001";
        if(stateProc2)
            args<<"-i"<<"udp://127.0.0.1:5002";
        if(stateProc3)
            args<<"-i"<<"udp://127.0.0.1:5003";

        int iCam1 = (stateProc1 ? 1 : 0);
        int iCam2 = (stateProc2 ? 2 : 0);
        int iCam3 = (stateProc3 ? 7 : 0);
        int iCams = iCam1 + iCam2 + iCam3;

        switch(iCams){
        case 1:
            args<<"-vcodec"<<"mpeg2video";
            break;
        case 8:
            args<<"-map"<<"0"<<"-map"<<"1:v"<<"-c:v:0"<<"mpeg2video"<<"-c:v:1"<<"copy";
            break;
        case 3:
            args<<"-map"<<"0"<<"-map"<<"1:v"<<"-c:v:0"<<"mpeg2video"<<"-c:v:1"<<"copy";
            break;
        case 10:
            args<<"-map"<<"0"<<"-map"<<"1:v"<<"-map"<<"2:v"<<"-c:v:0"<<"mpeg2video"<<"-c:v:1"<<"copy"<<"-c:v:2"<<"copy";
            break;
        case 2:
            args<<"-vcodec"<<"copy";
            break;
        case 7:
            args<<"-vcodec"<<"copy";
            break;
        case 9:
            args<<"-map"<<"0"<<"-map"<<"1:v"<<"-c:v:0"<<"copy"<<"-c:v:1"<<"copy";
            break;
        }

        args<<"-b"<<"10M";
        args<<file + ".mp4";
        qDebug()<<"args = "<<args;
        ui->btnPlay->setEnabled(false);
        ui->btnPlayLast->setEnabled(false);
        procRecord.setArguments(args);
        procRecord.start();
    }
}

void VideoReplayControl::stopRecord()
{
    ui->btnPlay->setEnabled(true);
    ui->btnPlayLast->setEnabled(true);
    if(procRecord.state() == QProcess::Running){
        qDebug()<<"rec start stop";
        procRecord.write("q");
        procRecord.waitForFinished();
        //if(procRecord.state() == QProcess::NotRunning){

        //}
    }
}

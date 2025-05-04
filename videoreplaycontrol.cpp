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

    connect(ui->btn, &QPushButton::clicked, this, [this](){
        killFfmpegProcess();
    });

    timerCam1 = new QTimer(this);
    connect(timerCam1, &QTimer::timeout, this, [this](){
        if(++countCam1 > 10){
            procReadCam1->write("q");
            procReadCam1->waitForBytesWritten();
            procReadCam1->closeWriteChannel();
            procReadCam1->close();
        }
    });

    timerCam2 = new QTimer(this);
    connect(timerCam2, &QTimer::timeout, this, [this](){
        if(++countCam2 > 10){
            procReadCam2->write("q");
            procReadCam2->waitForBytesWritten();
            procReadCam2->closeWriteChannel();
            procReadCam2->close();
        }
    });

    timerCam3 = new QTimer(this);
    connect(timerCam3, &QTimer::timeout, this, [this](){;
        if(++countCam3 > 10){
            procReadCam3->write("q");
            procReadCam3->waitForBytesWritten();
            procReadCam3->closeWriteChannel();
            procReadCam3->close();
        }
    });

    procReadCam1 = new QProcess(this);
    procReadCam1->setProgram("ffmpeg");
    connect(procReadCam1, &QProcess::readyReadStandardOutput, this, [this](){
        QByteArray ba = procReadCam1->readAllStandardOutput();
        QImage image(100, 50, QImage:: Format_RGB666);
        if (!image.loadFromData(ba, "PNG"))
            qDebug()<<"Not loaded";
        ui->lblCam1->setPixmap(QPixmap::fromImage(image));
        countCam1 = 0;
    });

    connect(procReadCam1, &QProcess::readyReadStandardError, this, [this](){
        QByteArray ba = procReadCam1->readAllStandardError();
        QFile file("camera1.txt");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        file.write(ba, ba.length());
        file.close();
        QMessageBox msgbox;
        msgbox.setText(QString(ba));
        msgbox.exec();
    });

    connect(procReadCam1, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            [=](int exitCode, QProcess::ExitStatus exitStatus){
        qDebug()<<"exitCode = "<<exitCode<<"exitStatus = "<<exitStatus;
        timerCam1->stop();
        countCam1 = 0;
        ui->cbCam1->setChecked(false);
        ui->lblCam1->clear();
        //stopRecord();
        if(camToVk == 1)
            offStreamVk();
    });

    procReadCam2 = new QProcess(this);
    procReadCam2->setProgram("ffmpeg");
    connect(procReadCam2, &QProcess::readyReadStandardOutput, this, [this](){
        QByteArray ba = procReadCam2->readAllStandardOutput();
        QImage image(100, 50, QImage:: Format_RGB666);
        if (!image.loadFromData(ba, "PNG"))
            qDebug()<<"Not loaded";
        ui->lblCam2->setPixmap(QPixmap::fromImage(image));
        countCam2 = 0;
    });

    connect(procReadCam2, &QProcess::readyReadStandardError, this, [this](){
        QByteArray ba = procReadCam2->readAllStandardError();
        QFile file("camera2.txt");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        file.write(ba, ba.length());
        file.close();
        QMessageBox msgbox;
        msgbox.setText(QString(ba));
        msgbox.exec();
    });
    connect(procReadCam2, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this](){
        timerCam2->stop();
        countCam2 = 0;
        ui->cbCam2->setChecked(false);
        ui->lblCam2->clear();
        if(camToVk == 2)
            offStreamVk();
    });

    procReadCam3 = new QProcess(this);
    procReadCam3->setProgram("ffmpeg");
    connect(procReadCam3, &QProcess::readyReadStandardOutput, this, [this](){
        QByteArray ba = procReadCam3->readAllStandardOutput();
        QImage image(100, 50, QImage:: Format_RGB666);
        if (!image.loadFromData(ba, "PNG"))
            qDebug()<<"Not loaded";
        ui->lblCam3->setPixmap(QPixmap::fromImage(image));
        countCam3 = 0;
    });

    connect(procReadCam3, &QProcess::readyReadStandardError, this, [this](){
        QByteArray ba = procReadCam3->readAllStandardError();
        QFile file("camera3.txt");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        file.write(ba, ba.length());
        file.close();
        QMessageBox msgbox;
        msgbox.setText(QString(ba));
        msgbox.exec();
    });

    connect(procReadCam3, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this](){
        timerCam3->stop();
        countCam3 = 0;
        ui->cbCam3->setChecked(false);
        ui->lblCam3->clear();
        if(camToVk == 3)
            offStreamVk();
    });

    procRecord = new QProcess(this);
    procRecord->setProgram("ffmpeg");
    connect(procRecord, &QProcess::readyReadStandardError, this, [this](){
        QByteArray s = procRecord->readAllStandardError();
        QFile file("record.txt");
        if (!file.open(QIODevice::Append | QIODevice::Text))
            return;
        file.write(s, s.length());
        file.close();
    });

    connect(procRecord, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            [=](/*int exitCode, QProcess::ExitStatus exitStatus*/){
                //qDebug()<<"exitCode = "<<exitCode<<"exitStatus = "<<exitStatus;
                ui->label->setStatusRec(false);
            });
    connect(procRecord, &QProcess::started, this, [=](){ui->label->setStatusRec(true);});

    procVk = new QProcess(this);
    procVk->setProgram("ffmpeg");
    connect(procVk, &QProcess::readyReadStandardError, this, [this](){
        QByteArray s = procVk->readAllStandardError();
        QFile file("vk.txt");
        if (!file.open(QIODevice::Append | QIODevice::Text))
            return;
        file.write(s, s.length());
        file.close();
        //procVk->write("q");
    });
    connect(procVk, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            [=](int exitCode, QProcess::ExitStatus exitStatus){
                qDebug()<<"vk exitCode = "<<exitCode<<"vk exitStatus = "<<exitStatus;
                ui->labelvk->setStatusVk(false);
            });
    connect(procVk, &QProcess::started, this, [=](){
        ui->labelvk->setStatusVk(true);
    });
    // connect(procVk, &QProcess::bytesWritten, this, [=](int bytes){
    //     qDebug()<<"bytes written = "<<bytes;
    // });

    connect(ui->btnPlay, &QPushButton::clicked, this, [this](){
        stopRecord();
        QString file = QFileDialog::getOpenFileName(nullptr, "Выбор видео", "videos");
        if(file == "" || !file.endsWith(".mp4"))
            return;
        slowMotionPlayer = new PlayerViewer(file);
        ui->btnPlay->setEnabled(false);
        ui->btnPlayLast->setEnabled(false);
        connect(slowMotionPlayer, &PlayerViewer::sigClose, this, [=](){slowMotionPlayer->deleteLater();});
        //connect(slowMotionPlayer, &PlayerViewer::sigClose, this, &VideoReplayControl::sigHidePlayer);
        connect(slowMotionPlayer, &PlayerViewer::sigImage, this, &VideoReplayControl::sigImage);
        connect(slowMotionPlayer, &PlayerViewer::sigClose, this, [this](){
            emit sigHidePlayer();
            ui->btnPlay->setEnabled(true);
            ui->btnPlayLast->setEnabled(true);
        });
    });

    connect(ui->btnPlayLast, &QPushButton::clicked, this, [this](){
        stopRecord();
        QDir dir("videos");
        QStringList dirList = dir.entryList(QDir::Files, QDir::Time);
        if(dirList.count() == 0)
            return;
        slowMotionPlayer = new PlayerViewer("videos/" + dirList.at(0));
        ui->btnPlay->setEnabled(false);
        ui->btnPlayLast->setEnabled(false);
        connect(slowMotionPlayer, &PlayerViewer::sigClose, this, [=](){slowMotionPlayer->deleteLater();});
        //connect(slowMotionPlayer, &PlayerViewer::sigClose, this, &VideoReplayControl::sigHidePlayer);
        connect(slowMotionPlayer, &PlayerViewer::sigImage, this, &VideoReplayControl::sigImage);
        connect(slowMotionPlayer, &PlayerViewer::sigClose, this, [this](){
            emit sigHidePlayer();
            ui->btnPlay->setEnabled(true);
            ui->btnPlayLast->setEnabled(true);
        });

        emit sigShowPlayer();

    });

    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.beginGroup("URL");
    urlCam2 = settings.value("cam1", "").toString();
    urlCam3 = settings.value("cam2", "").toString();
    settings.endGroup();

    getHWcodec();
}

VideoReplayControl::~VideoReplayControl()
{
    qDebug()<<"~VideoReplayControl()";
    if(procRecord->state() == QProcess::Running){
        procRecord->write("q");
        procRecord->waitForBytesWritten();
        procRecord->closeWriteChannel();
        procRecord->waitForFinished();
    }
    if(procReadCam1->state() == QProcess::Running){
        procReadCam1->write("q");
        procReadCam1->waitForBytesWritten();
        procReadCam1->closeWriteChannel();
        procReadCam1->waitForFinished();
    }
    if(procReadCam2->state() == QProcess::Running){
        procReadCam2->write("q");
        procReadCam2->waitForBytesWritten();
        procReadCam2->closeWriteChannel();
        procReadCam2->waitForFinished();
    }
    if(procReadCam3->state() == QProcess::Running){
        procReadCam3->write("q");
        procReadCam3->waitForBytesWritten();
        procReadCam3->closeWriteChannel();
        procReadCam3->waitForFinished();
    }

    killFfmpegProcess();

    delete ui;
}

void VideoReplayControl::turnCam1(bool check)
{
    if(check){
        if(procReadCam1->state() == QProcess::NotRunning){
            QStringList args;
            QString url;
            if(urlSound != "")
                url = "video=" + urlCam1 + ":audio=" + urlSound;

            args<<"-loglevel"<<"error";

            if(hwDecoder != "")
                args<<"-c:v"<<hwDecoder;

            args<<"-f"<<"dshow"<<"-rtbufsize"<<"2000M"<<"-framerate"<<fps
                 <<"-video_size"<<resolution;

            args<<"-i"<<url;

            if(streamToVk && camToVk == 1){
                args<<"-f"<<"gdigrab"<<"-framerate"<<"1"<<"-i"<<"title=TabloOnTv";
                args<<"-filter_complex"<<"[1]scale=" + widthPipVk + ":" + heightPipVk + ", colorchannelmixer=aa=" + transparentPipVk + " [pip]; "
                        "[0][pip] overlay=main_w-overlay_w-10:main_h-overlay_h-10[out_vk]; "
                        "[0:v]scale=100:50, fps=1 [vout]; "
                        "[out_vk] drawtext=text='DEMO':x=(w / 2-text_w / 2):y=(h / 2-text_h / 2)"
                        ":fontfile=arial.ttf:fontsize=240:fontcolor=red";
                args<<"-b:v"<<"4M"<<"-f"<<"flv"<<"udp://127.0.0.1:5004";    //for stream to vk
            }
            else{
                args<<"-filter_complex"<<"[0:v]scale=100:50, fps=1 [vout]";
            }

            args<<"-map"<<"[vout]"<<"-vcodec"<<"png"<<"-f"<<"image2pipe"<<"-";

            QString codec = hwEncoder == "" ? "mpeg2video" : hwEncoder;

            args<<"-map"<<"0"<<"-b:v"<<"4M"<<"-vcodec"<<codec<<"-f"<<"mpegts"<<"udp://127.0.0.1:5001";

            procReadCam1->setArguments(args);
            procReadCam1->start();
            procReadCam1->waitForStarted();
            timerCam1->start(1000);
            qDebug()<<args;
            if(camToVk == 1)
                onStreamVk();
        }
    }
    else{
        if(procReadCam1->state() == QProcess::Running){
            procReadCam1->write("q");
        }
    }
}

void VideoReplayControl::turnCam2(bool check)
{
    qDebug()<<"turn cam2"<<check;
    if(check){
        if(procReadCam2->state() == QProcess::NotRunning){
            QStringList args;
            args<<"-hide_banner"<<"-loglevel"<<"error";
            if(urlCam2.startsWith("rtsp"))
               args<<"-rtsp_transport"<<"tcp";
            //args<<"-vsync"<<"vfr";
            args<<"-i"<<urlCam2;
            if(streamToVk && camToVk == 2){
                args<<"-f"<<"gdigrab"<<"-framerate"<<"1"<<"-i"<<"title=TabloOnTv";
                args<<"-filter_complex"<<"[1]scale=" + widthPipVk + ":" + heightPipVk + ", colorchannelmixer=aa=" + transparentPipVk + " [pip]; "
                        "[0][pip] overlay=main_w-overlay_w-10:main_h-overlay_h-10[out_vk]; "
                        "[0:v]scale=100:50, fps=1 [vout]; "
                        "[out_vk] drawtext=text='DEMO':x=(w / 2-text_w / 2):y=(h / 2-text_h / 2)"
                        ":fontfile=arial.ttf:fontsize=240:fontcolor=red";
                args<<"-f"<<"flv"<<"udp://127.0.0.1:5004";
            }
            else{
                args<<"-filter_complex"<<"[0:v]scale=100:50, fps=1 [vout]";
            }
            args<<"-map"<<"[vout]"<<"-vcodec"<<"png"<<"-f"<<"image2pipe"<<"-";
            args<<"-map"<<"0"<<"-vcodec"<<"copy"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5002";
            qDebug()<<"args 2 = "<<args;
            procReadCam2->setArguments(args);
            procReadCam2->start();
            timerCam2->start(1000);
            procReadCam2->waitForStarted();
            if(camToVk == 2)
                onStreamVk();
        }
    }
    else{
        if(procReadCam2->state() == QProcess::Running)
            procReadCam2->write("q");
    }
}

void VideoReplayControl::turnCam3(bool check)
{
    if(check){
        if(procReadCam3->state() == QProcess::NotRunning){
            QStringList args;
            args<<"-hide_banner"<<"-loglevel"<<"error";
            if(urlCam3.startsWith("rtsp"))
                args<<"-rtsp_transport"<<"tcp";
            args<<"-i"<<urlCam3;
            if(streamToVk && camToVk == 3){
                args<<"-f"<<"gdigrab"<<"-framerate"<<"1"<<"-i"<<"title=TabloOnTv";
                args<<"-filter_complex"<<"[1]scale=" + widthPipVk + ":" + heightPipVk + ", colorchannelmixer=aa=" + transparentPipVk + " [pip]; "
                        "[0][pip] overlay=main_w-overlay_w-10:main_h-overlay_h-10[out_vk]; "
                        "[0:v]scale=100:50, fps=1 [vout]; "
                        "[out_vk] drawtext=text='DEMO':x=(w / 2-text_w / 2):y=(h / 2-text_h / 2)"
                        ":fontfile=arial.ttf:fontsize=240:fontcolor=red";
                args<<"-map"<<"0"<<"-f"<<"flv"<<"udp://127.0.0.1:5004";
            }
            else{
                args<<"-filter_complex"<<"[0:v]scale=100:50, fps=1 [vout]";
            }
            args<<"-map"<<"[vout]"<<"-vcodec"<<"png"<<"-f"<<"image2pipe"<<"-";
            args<<"-map"<<"0"<<"-vcodec"<<"copy"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5003";
            procReadCam3->setArguments(args);
            procReadCam3->start();
            timerCam3->start(1000);
            procReadCam3->waitForStarted();
            qDebug()<<args;

            if(camToVk == 3)
                onStreamVk();
        }
    }
    else{
        if(procReadCam3->state() == QProcess::Running)
            procReadCam3->write("q");
        //()<<"off1";
        //offStreamVk3();
    }
}

void VideoReplayControl::killFfmpegProcess()
{
    QProcess findProc(this);
    findProc.setProgram("cmd");
    QStringList args;
    args<<"/c"<<"taskkill"<<"/im"<<"ffmpeg.exe"<<"/f";
    findProc.setArguments(args);
    findProc.start();
    findProc.waitForFinished();
    findProc.close();
}

void VideoReplayControl::offStreamVk()
{
    qDebug()<<"finVk"<<camToVk;
    if(procVk->state() == QProcess::Running){
        procVk->write("q");
        procVk->waitForBytesWritten();
        procVk->closeWriteChannel();
        //procVk->waitForFinished();
        procVk->close();
    }
}

void VideoReplayControl::getHWcodec()
{
    QProcess proc(this);
    proc.setProgram("ffmpeg");
    QStringList args;
    args<<"-y"<<"-loglevel"<<"error"<<"-f"<<"lavfi"<<"-i"<<"nullsrc"<<"-frames:v"<<"1"<<"-c:v"<<"h264_nvenc"<<"test.mp4";
    proc.setArguments(args);
    proc.start();
    proc.waitForFinished();
    QByteArray ba = proc.readAllStandardError();
    if(ba.count() == 0){
        proc.close();
        hwEncoder = "h264_nvenc";
        hwDecoder = "mjpeg_cuvid";
        return;
    }
    args.clear();
    args<<"-y"<<"-loglevel"<<"error"<<"-f"<<"lavfi"<<"-i"<<"nullsrc"<<"-frames:v"<<"1"<<"-c:v"<<"h264_qsv"<<"test.mp4";
    proc.setArguments(args);
    proc.start();
    proc.waitForFinished();
    ba = proc.readAllStandardError();
    if(ba.count() == 0){
        proc.close();
        hwEncoder = "h264_qsv";
        hwDecoder = "mjpeg_qsv";
        return;
    }
    proc.close();
    return;
}

void VideoReplayControl::onStreamVk()
{
    qDebug()<<"cam = "<<camToVk;
    if(streamToVk && procVk->state() == QProcess::NotRunning){
        QStringList args;
        args<<"-loglevel"<<"error"<<
            "-i"<<"udp://127.0.0.1:5004"<<"-vcodec"<<"copy"<<"-f"<<"flv"<<urlVk + keyVk;
        procVk->setArguments(args);
        procVk->start();
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
    QStringList args;
    if(procRecord->state() == QProcess::NotRunning){
        bool stateProc1 = procReadCam1->state() == QProcess::Running ? true : false;
        bool stateProc2 = procReadCam2->state() == QProcess::Running ? true : false;
        bool stateProc3 = procReadCam3->state() == QProcess::Running ? true : false;
        if(!(stateProc1 || stateProc2 || stateProc3))
            return;

        //args<<"-loglevel"<<"error";
        //args<<"-hwaccel"<<"auto";

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
            args<<"-vcodec"<<"copy";
            break;
        case 8:
            args<<"-map"<<"0"<<"-r"<<"30"<<"-map"<<"1:v"<<"-r"<<"30"<<"-c:v:0"<<hwEncoder<<"-c:v:1"<<"copy";
            break;
        case 3:
            args<<"-map"<<"0"<<"-map"<<"1:v"<<"-c:v:0"<<hwEncoder<<"-c:v:1"<<"copy";
            break;
        case 10:
            args<<"-map"<<"0"<<"-map"<<"1:v"<<"-map"<<"2:v"<<"-c:v:0"<<hwEncoder<<"-c:v:1"<<"copy"<<"-c:v:2"<<"copy";
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
        procRecord->setArguments(args);
        procRecord->start();
    }
}

void VideoReplayControl::stopRecord()
{
    ui->btnPlay->setEnabled(true);
    ui->btnPlayLast->setEnabled(true);
    if(procRecord->state() == QProcess::Running){
        qDebug()<<"stopRecord0";
        procRecord->write("q");
        procRecord->waitForBytesWritten();
        procRecord->closeWriteChannel();
        qDebug()<<"procRecord->waitForFinished(1000) = "<<procRecord->waitForFinished(1000);
        procRecord->close();
        qDebug()<<"stopRecord1";
    }
}

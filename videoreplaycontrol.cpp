#include "videoreplaycontrol.h"
#include "mainwindow.h"
#include "qdatetime.h"
#include "qdebug.h"
#include "qdir.h"
#include "qmessagebox.h"
#include "qsettings.h"
#include <QFile>
#include "ui_videoreplaycontrol.h"
#include <QFileDialog>

VideoReplayControl::VideoReplayControl(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VideoReplayControl)
{
    ui->setupUi(this);
    setAutoFillBackground(true);

    killFfmpegProcess();

    btnPlay = new SvgButton(":/images/play_choice_enable.svg", ":/images/play_choice_disable.svg", this);
    btnPlay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->hLayoutButtons->insertWidget(0, btnPlay);
    btnPlay->setMinimumHeight(50);

    btnPlayLast = new SvgButton(":/images/play_last_enable.svg", ":/images/play_last_disable.svg", this);
    btnPlayLast->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->hLayoutButtons->insertWidget(1, btnPlayLast);

    player = new PlayerPc;

    connect(ui->cbCam1, &QCheckBox::clicked, this, [this](){
        if(procRead->state() == QProcess::Running)
            stopReadCams();
        else
            startReadCams();

    });
    connect(ui->cbCam2, &QCheckBox::clicked, this, [this](){
        if(procRead->state() == QProcess::Running)
            stopReadCams();
        else
            startReadCams();

    });
    connect(ui->cbCam3, &QCheckBox::clicked, this, [this](){
        if(procRead->state() == QProcess::Running)
            stopReadCams();
        else
            startReadCams();

    });

    timerCam1 = new QTimer(this);
    connect(timerCam1, &QTimer::timeout, this, [this](){
        if(countCam1 < 11)
            countCam1 += 1;
        if(countCam1 == 10)
            ui->lblCam1->clear();
    });

    timerCam2 = new QTimer(this);
    connect(timerCam2, &QTimer::timeout, this, [this](){
        if(countCam2 < 11)
            countCam2 += 1;
        if(countCam2 == 10)
            ui->lblCam2->clear();
    });

    timerCam3 = new QTimer(this);
    connect(timerCam3, &QTimer::timeout, this, [this](){
        if(countCam3 < 11)
            countCam3 += 1;
        if(countCam3 == 10)
            ui->lblCam3->clear();
    });

    QStringList args;

    procRead = new QProcess(this);
    procRead->setProgram("ffmpeg");
    connect(procRead, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            [=](int exitCode, QProcess::ExitStatus exitStatus){
                qDebug()<<"exitCodeRead = "<<exitCode<<"exitStatusRead = "<<exitStatus;
                startReadCams();
            });
    connect(procRead, &QProcess::started, this, [this](){
        if(streamToVk && procVk->state() == QProcess::NotRunning){
            if(camToVk == 1 && ui->cbCam1->isChecked()){
                procProbeAudio->start();
                qDebug()<<"procProbeAudio cam1";
            }
            if(camToVk == 2 && ui->cbCam2->isChecked())
                procProbeAudio->start();
            if(camToVk == 3 && ui->cbCam3->isChecked())
                procProbeAudio->start();
        }
    });
    connect(procRead, &QProcess::readyReadStandardError, this, [this](){
        QByteArray ba = procRead->readAllStandardError();
        QMessageBox box;
        qDebug()<<"err read = "<<ba;

        if(QString(ba).contains(urlCam2)){
            ui->cbCam2->setChecked(false);
            ui->lblCam2->clear();
        }

        if(QString(ba).contains(urlCam3)){
            ui->cbCam3->setChecked(false);
            ui->lblCam3->clear();
        }

        if(!ba.contains("I/O error"))
            return;
        QString url(ba);
        int index = url.lastIndexOf(":");

        url = url.remove(index, 20);
        box.setText(url);
        box.exec();

        QString url1;
        url1 = url.split(":").at(0);
        url1 = url1.remove(0, 6);

        if(url1 == urlCam1){
            ui->cbCam1->setChecked(false);
            ui->lblCam1->clear();
        }
        if(url == urlCam2){
            ui->cbCam2->setChecked(false);
            ui->lblCam2->clear();
        }
        if(url == urlCam3){
            ui->cbCam3->setChecked(false);
            ui->lblCam3->clear();
        }
    });

    procReadCam1 = new QProcess(this);
    procReadCam1->setProgram("ffmpeg");
    args<<"-i"<<"udp://127.0.0.1:5001"<<"-filter_complex"<<"[0:v]scale=100:50, fps=5 "<<"-vcodec"<<"png"<<"-f"<<"image2pipe"<<"-";
    procReadCam1->setArguments(args);
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
    });

    procReadCam2 = new QProcess(this);
    procReadCam2->setProgram("ffmpeg");
    args.clear();
    args<<"-i"<<"udp://127.0.0.1:5002"<<"-filter_complex"<<"[0:v]scale=100:50, fps=5 "<<"-vcodec"<<"png"<<"-f"<<"image2pipe"<<"-";
    procReadCam2->setArguments(args);
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
    });

    procReadCam3 = new QProcess(this);
    procReadCam3->setProgram("ffmpeg");
    args.clear();
    args<<"-i"<<"udp://127.0.0.1:5003"<<"-filter_complex"<<"[0:v]scale=100:50, fps=5 "<<"-vcodec"<<"png"<<"-f"<<"image2pipe"<<"-";
    procReadCam3->setArguments(args);
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
    });

    procRecord = new QProcess(this);
    procRecord->setProgram("ffmpeg");
    connect(procRecord, &QProcess::readyReadStandardError, this, [this](){
        QByteArray ba = procRecord->readAllStandardError();
        qDebug()<<"err rec ="<<ba;
        QFile file("record.txt");
        if (!file.open(QIODevice::Append | QIODevice::Text))
            return;
        file.write(ba, ba.length());
        file.close();

        if(QString(ba).contains(urlCam2)){
            ui->cbCam2->setChecked(false);
            ui->lblCam2->clear();
        }

        if(QString(ba).contains(urlCam3)){
            ui->cbCam3->setChecked(false);
            ui->lblCam3->clear();
        }

        if(!ba.contains("I/O error"))
            return;
        QMessageBox box;
        QString url(ba);
        int index = url.lastIndexOf(":");

        url = url.remove(index, 20);
        box.setText(url);
        box.exec();

        QString url1;
        url1 = url.split(":").at(0);
        url1 = url1.remove(0, 6);

        if(url1 == urlCam1)
            ui->cbCam1->setChecked(false);
        if(url == urlCam2)
            ui->cbCam2->setChecked(false);
        if(url == urlCam3)
            ui->cbCam3->setChecked(false);

    });

    connect(procRecord, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            [=](int exitCode, QProcess::ExitStatus exitStatus){
                qDebug()<<"exitCodeRec = "<<exitCode<<"exitStatus = "<<exitStatus;
                ui->label->setStatusRec(false);
                ui->cbCam1->setEnabled(true);
                ui->cbCam2->setEnabled(true);
                ui->cbCam3->setEnabled(true);
            });
    connect(procRecord, &QProcess::started, this, [=](){
        ui->cbCam1->setEnabled(false);
        ui->cbCam2->setEnabled(false);
        ui->cbCam3->setEnabled(false);
        ui->label->setStatusRec(true);
    });

    procVk = new QProcess(this);
    procVk->setProgram("ffmpeg");
    connect(procVk, &QProcess::readyReadStandardError, this, [this](){
        QByteArray s = procVk->readAllStandardError();
        qDebug()<<"err vk =  "<<s;
        if(s.contains("I/O error"))
            procVk->write("q");
        // QString sFrame = QString(s);
        // if(sFrame.startsWith("frame= ")){
        //     int countFrame = sFrame.split(" ").at(1).toInt();
        //     if(countFrame != 0){
        //         if(countFrame > lastCountFrame){
        //             lastCountFrame = countFrame;
        //             countVk = 0;
        //             qDebug()<<"countFrame = "<<countFrame;
        //         }
        //     }
        // }
    });
    connect(procVk, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            [=](int exitCode, QProcess::ExitStatus exitStatus){
                qDebug()<<"vk exitCode = "<<exitCode<<"vk exitStatus = "<<exitStatus;
                ui->labelvk->setStatusVk(false);
            });
    connect(procVk, &QProcess::started, this, [=](){
        lastCountFrame = 0;
        ui->labelvk->setStatusVk(true);
    });
    // connect(procVk, &QProcess::bytesWritten, this, [=](int bytes){
    //     qDebug()<<"bytes written = "<<bytes;
    // });

    connect(btnPlay, &QPushButton::clicked, this, [this](){
        stopRecord();
        QString file = QFileDialog::getOpenFileName(nullptr, "Выбор видео", "videos");
        if(file == "" || !file.endsWith(".mp4"))
            return;
        QThread::sleep(1);
        player->setMediaUrl(file);
        player->showFullScreen();
        btnPlay->setEnabled(false);
        btnPlayLast->setEnabled(false);
        emit sigShowPlayer();

    });

    connect(btnPlayLast, &QPushButton::clicked, this, [this](){
        stopRecord();
        QDir dir("videos");
        QStringList dirList = dir.entryList(QDir::Files, QDir::Time);
        if(dirList.count() == 0)
            return;
        QThread::sleep(1);
        player->setMediaUrl("videos/" + dirList.at(0));
        player->showFullScreen();
        emit sigShowPlayer();

        btnPlay->setEnabled(false);
        btnPlayLast->setEnabled(false);

    });

    connect(player, &PlayerPc::sigClose, this, [this](){
        emit sigHidePlayer();
        btnPlay->setEnabled(true);
        btnPlayLast->setEnabled(true);
    });

    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.beginGroup("URL");
    urlCam2 = settings.value("cam1", "").toString();
    urlCam3 = settings.value("cam2", "").toString();
    settings.endGroup();

    getHWcodec();

    p = parent;

    procReadCam1->start();
    timerCam1->start(500);
    procReadCam2->start();
    timerCam2->start(500);
    procReadCam3->start();
    timerCam3->start(500);

    procProbeAudio = new QProcess(this);
    procProbeAudio->setProgram("ffprobe");
    procProbeAudio->setArguments({"-hide_banner", "-i", "udp://127.0.0.1:5004"});
    procProbeAudio->setReadChannel(QProcess::StandardError);
    connect(procProbeAudio,  QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this](){
        isAudio = false;
        qDebug()<<"procProbeAudio";
        while(true){
            QByteArray ba = procProbeAudio->readLine();
            qDebug()<<"ba = "<<ba;
            //if(ba.contains("error"))
            //    break;
            if(ba.size() == 0)
                break;
            if(ba.contains("Stream") && ba.contains("Audio"))
                isAudio = true;

        }
        onStreamVk();
    });

}

VideoReplayControl::~VideoReplayControl()
{

    killFfmpegProcess();
    delete player;
    delete ui;
}

void VideoReplayControl::setPlayerTv(QMediaPlayer* p)
{
    player->setTvPlayer(p);
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
        //procVk->close();
    }
}

void VideoReplayControl::getHWcodec()
{
    QProcess proc(this);
    proc.setProgram("ffmpeg");
    QStringList decoders = {"mjpeg_cuvid", "mjpeg_amf", "mjpeg_qsv"};
    QStringList encoders = {"h264_nvenc", "h264_amf", "h264_qsv"};
    foreach(auto each, encoders){
        QStringList args({"-y", "-loglevel", "error", "-f", "lavfi", "-i", "nullsrc", "-frames:v", "1", "-c:v", each, "test.mp4"});
        proc.setArguments(args);
        proc.start();
        proc.waitForFinished();
        QByteArray ba = proc.readAllStandardError();
        if(ba.size() == 0){
            hwEncoder = each;
            qDebug()<<"hwEncoder = "<<hwEncoder;
            break;
        }
    }
    foreach(auto each, decoders){
        QStringList args({"-y", "-loglevel", "error", "-c:v", each, "-f", "mjpeg", "-i", "test.jpg", "-frames:v", "1", "test.mp4"});
        proc.setArguments(args);
        proc.start();
        proc.waitForFinished();
        QByteArray ba = proc.readAllStandardError();
        if(ba.size() == 0){
            hwDecoder = each;
            qDebug()<<"hwDecoder = "<<hwDecoder;
            break;
        }
    }

    proc.close();
    return;
}

void VideoReplayControl::startReadCams()
{
    //if(procRead->state() == QProcess::Running)
    //    stopReadCams();

    int isCam1 = ui->cbCam1->isChecked() ? 1 : 0;
    int isCam2 = ui->cbCam2->isChecked() ? 2 : 0;
    int isCam3 = ui->cbCam3->isChecked() ? 4 : 0;

    QStringList argsInput1;
    QStringList argsInput2;
    QStringList argsInput3;

    int turnOnCams = isCam1 + isCam2 + isCam3;
    if(turnOnCams == 0)
        return;

    QString codec = hwEncoder == "" ? "mpeg2video" : hwEncoder;
    QStringList args;
    QString url = "video=" + urlCam1;
    if(urlSound != "")
        url += ":audio=" + urlSound;

    if(hwDecoder != "")
        argsInput1<<"-c:v"<<hwDecoder;
    argsInput1<<"-f"<<"dshow"<<"-rtbufsize"<<"2000M"<<"-framerate"<<fps<<"-video_size"<<resolution
               <<"-i"<<url;

    argsInput2<<"-rtbufsize"<<"2000M";
    if(urlCam2.startsWith("rtsp"))
        argsInput2<<"-rtsp_transport"<<"tcp"<<"-stimeout"<<"10000";
    argsInput2<<"-i"<<urlCam2;

    argsInput3<<"-rtbufsize"<<"2000M";
    if(urlCam3.startsWith("rtsp"))
        argsInput3<<"-rtsp_transport"<<"tcp"<<"-stimeout"<<"10000";
    argsInput3<<"-i"<<urlCam3;

    args<<"-hide_banner"<<"-loglevel"<<"error";

    switch(turnOnCams){
    case 1:             //1
        args += argsInput1;
        args<<"-map"<<"0"<<"-vcodec"<<codec<<"-b:v"<<"5M"<<"-g"<<"1"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5000";
        args<<"-map"<<"0:v"<<"-vcodec"<<codec<<"-b:v"<<"1M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5001";
        if(streamToVk && camToVk == 1)
            args<<"-map"<<"0"<<"-g"<<"1"<<"-vcodec"<<codec<<"-b"<<"3M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
        break;
    case 2:             //2
        args += argsInput2;
        args<<"-map"<<"0"<<"-vcodec"<<"copy"<<"-g"<<"1"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5000";
        args<<"-map"<<"0:v"<<"-vcodec"<<"copy"<<"-b:v"<<"1M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5002";
        if(streamToVk && camToVk == 2)
            args<<"-map"<<"0"<<"-vcodec"<<"copy"<<"-b"<<"3M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
        break;
    case 4:             //3   
        args += argsInput3;
        args<<"-map"<<"0"<<"-vcodec"<<"copy"<<"-g"<<"1"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5000";
        args<<"-map"<<"0:v"<<"-vcodec"<<"copy"<<"-b:v"<<"1M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5003";
        if(streamToVk && camToVk == 3)
            args<<"-map"<<"0"<<"-vcodec"<<"copy"<<"-b"<<"3M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
        break;
    case 3:             //1 & 2
        args += argsInput1;
        args += argsInput2;
        args<<"-map"<<"0"<<"-b"<<"5M";
        args<<"-map"<<"1";
        args<<"-g"<<"1"<<"-c:v:0"<<codec<<"-c:v:1"<<"copy"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5000";
        args<<"-map"<<"0:v"<<"-vcodec"<<codec<<"-b:v"<<"1M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5001";
        args<<"-map"<<"1:v"<<"-vcodec"<<"copy"<<"-b:v"<<"1M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5002";
        if(streamToVk && camToVk == 1)
            args<<"-map"<<"0"<<"-g"<<"1"<<"-vcodec"<<codec<<"-b"<<"3M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
        else if(streamToVk && camToVk == 2)
            args<<"-map"<<"1"<<"-vcodec"<<"copy"<<"-b"<<"3M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
        break;
    case 5:             //1 & 3
        args += argsInput1;
        args += argsInput3;
        args<<"-map"<<"0"<<"-b"<<"5M";
        args<<"-map"<<"1";
        args<<"-g"<<"1"<<"-c:v:0"<<codec<<"-c:v:1"<<"copy"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5000";
        args<<"-map"<<"0:v"<<"-vcodec"<<codec<<"-b:v"<<"1M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5001";
        args<<"-map"<<"1:v"<<"-vcodec"<<"copy"<<"-b:v"<<"1M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5003";
        if(streamToVk && camToVk == 1)
            args<<"-map"<<"0"<<"-g"<<"1"<<"-vcodec"<<codec<<"-b"<<"3M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
        else if(streamToVk && camToVk == 3)
            args<<"-map"<<"1"<<"-vcodec"<<"copy"<<"-b"<<"3M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
        break;
    case 6:             //2 & 3
        args += argsInput2;
        args += argsInput3;
        args<<"-map"<<"0";
        args<<"-map"<<"1";
        args<<"-g"<<"1"<<"-c:v:0"<<"copy"<<"-c:v:1"<<"copy"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5000";
        args<<"-map"<<"0:v"<<"-vcodec"<<"copy"<<"-b:v"<<"1M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5002";
        args<<"-map"<<"1:v"<<"-vcodec"<<"copy"<<"-b:v"<<"1M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5003";
        if(streamToVk && camToVk == 2)
            args<<"-map"<<"0"<<"-vcodec"<<"copy"<<"-b"<<"3M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
        else if(streamToVk && camToVk == 3)
            args<<"-map"<<"1"<<"-vcodec"<<"copy"<<"-b"<<"3M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
        break;
    case 7:             //1 & 2 & 3
        args += argsInput1;
        args += argsInput2;
        args += argsInput3;
        args<<"-map"<<"0"<<"-b"<<"5M";
        args<<"-map"<<"1";
        args<<"-map"<<"2";
        args<<"-g"<<"1"<<"-c:v:0"<<codec<<"-c:v:1"<<"copy"<<"-c:v:2"<<"copy"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5000";
        args<<"-map"<<"0:v"<<"-vcodec"<<codec<<"-b:v"<<"1M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5001";
        args<<"-map"<<"1:v"<<"-vcodec"<<"copy"<<"-b:v"<<"1M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5002";
        args<<"-map"<<"2:v"<<"-vcodec"<<"copy"<<"-b:v"<<"1M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5003";
        if(streamToVk && camToVk == 1)
            args<<"-map"<<"0"<<"-g"<<"1"<<"-vcodec"<<codec<<"-b"<<"3M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
        else if(streamToVk && camToVk == 2)
            args<<"-map"<<"1"<<"-vcodec"<<"copy"<<"-b"<<"3M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
        else if(streamToVk && camToVk == 3)
            args<<"-map"<<"2"<<"-vcodec"<<"copy"<<"-b"<<"3M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
        break;
    }

    qDebug()<<args;
    procRead->setArguments(args);
    procRead->start();


}

void VideoReplayControl::stopReadCams()
{
    procRead->write("q");
    procRead->closeWriteChannel();
}

void VideoReplayControl::onStreamVk()
{
    // if(procVk->state() == QProcess::Running)
    //     return;

    //if(streamToVk && procVk->state() == QProcess::NotRunning){
        QStringList args;
        args<<"-hide_banner"<<"-rtbufsize"<<"2000M"<<"-timeout"<<"1000";
        args<<"-i"<<"udp://127.0.0.1:5004"; //0
        args<<"-f"<<"gdigrab"<<"-framerate"<<"1"<<"-i"<<"title=TabloOnTv"; //1
        qDebug()<<"isAudio = "<<isAudio;
        if(!isAudio)
            args<<"-f"<<"dshow"<<"-i"<<"audio=" + urlSound; //2

        if(static_cast<MainWindow*>(p->parent())->getStatusRegistration()){
            if(isAudio){
                args<<"-filter_complex"<<"[1]scale=" + widthPipVk + ":" + heightPipVk + ", colorchannelmixer=aa=" + transparentPipVk + " [pip]; "
                                                                                                                      "[pip] setpts=PTS-STARTPTS+7/TB [sync_pip]; "
                                                                                                                      "[0][sync_pip] overlay=main_w-overlay_w-10:main_h-overlay_h-10 [out_vk]; "
                                                                                                                      "[out_vk] drawtext=text='DEMO':x=(w / 2-text_w / 2):y=(h / 2-text_h / 2):fontfile=arial.ttf:fontsize=240:fontcolor=red ";
            }
            else{
                args<<"-filter_complex"<<"[1]scale=" + widthPipVk + ":" + heightPipVk + ", colorchannelmixer=aa=" + transparentPipVk + " [pip]; "
                                                                                                                      "[pip] setpts=PTS-STARTPTS+7/TB [sync_pip]; "
                                                                                                                      "[0][sync_pip] overlay=main_w-overlay_w-10:main_h-overlay_h-10 [out_vk]; "
                                                                                                                      "[2] asetpts=PTS+5/TB [out_audio]; "
                                                                                                                      "[out_vk] drawtext=text='DEMO':x=(w / 2-text_w / 2):y=(h / 2-text_h / 2):fontfile=arial.ttf:fontsize=240:fontcolor=red [out]";
            }
        }
        else{
            if(isAudio){
                args<<"-filter_complex"<<"[1]scale=" + widthPipVk + ":" + heightPipVk + ", colorchannelmixer=aa=" + transparentPipVk + " [pip]; "
                                                                                                                      "[pip] setpts=PTS-STARTPTS+7/TB [sync_pip]; "
                                                                                                                      "[0][sync_pip] overlay=main_w-overlay_w-10:main_h-overlay_h-10 ";
            }
            else{
                args<<"-filter_complex"<<"[1]scale=" + widthPipVk + ":" + heightPipVk + ", colorchannelmixer=aa=" + transparentPipVk + " [pip]; "
                                                                                                                      "[pip] setpts=PTS-STARTPTS+7/TB [sync_pip]; "
                                                                                                                      "[2] asetpts=PTS+5/TB [out_audio]; "
                                                                                                                      "[0][sync_pip] overlay=main_w-overlay_w-10:main_h-overlay_h-10 [out]";
            }
        }
        if(isAudio)
            args<<"-b"<<"3M"<<"-f"<<"flv"<<"-flvflags"<<"no_duration_filesize"<<urlVk + keyVk;
        else
            args<<"-map"<<"[out]"<<"-map"<<"[out_audio]"<<"-b"<<"3M"<<"-f"<<"flv"<<"-flvflags"<<"no_duration_filesize"<<urlVk + keyVk;
        qDebug()<<"vk = "<<args;
        procVk->setArguments(args);
        procVk->start();
    //}
}

void VideoReplayControl::setWebCam(QString cam)
{
    urlCam1 = cam;
}

void VideoReplayControl::setParamWebCam(QList<int> param)
{
    fps = QString::number(param.at(0));
    resolution = QString::number(param.at(1)) + "x" + QString::number(param.at(2));
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
    btnPlay->setEnabled(!b);
    btnPlayLast->setEnabled(!b);

    if(procRead->state() == QProcess::NotRunning)
        return;

    if(procRecord->state() == QProcess::Starting || procRecord->state() == QProcess::Running)
        return;

    QString file = "videos/" + s + "_" + QTime::currentTime().toString("hh:mm:ss");
    file.replace(":", "_");

    QStringList args;
    args<<"-hide_banner"<<"-loglevel"<<"error"<<"-i"<<"udp://127.0.0.1:5000"<<"-map"<<"0"<<"-c:v"<<"copy"<<"-c:a"<<"aac"<<file + ".mp4";
    qDebug()<<args;
    procRecord->setArguments(args);
    procRecord->start();
}

void VideoReplayControl::stopRecord()
{
    btnPlay->setEnabled(true);
    btnPlayLast->setEnabled(true);
    if(procRecord->state() == QProcess::Running){
        procRecord->write("q");
        procRecord->waitForBytesWritten();
        procRecord->closeWriteChannel();
        procRecord->waitForFinished();
    }
}

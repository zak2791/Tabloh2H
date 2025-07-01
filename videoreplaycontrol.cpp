#include "videoreplaycontrol.h"
#include "playertv.h"
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
        if(countCam1 < 61)
            countCam1 += 1;
        if(countCam1 == 5)
            ui->lblCam1->clear();
        if(countCam1 == 60){
            ui->cbCam1->setChecked(false);
        }
    });

    timerCam2 = new QTimer(this);
    connect(timerCam2, &QTimer::timeout, this, [this](){
        if(countCam2 < 61)
            countCam2 += 1;
        if(countCam2 == 5)
            ui->lblCam2->clear();
        if(countCam2 == 60){
            ui->cbCam2->setChecked(false);
        }
    });

    timerCam3 = new QTimer(this);
    connect(timerCam3, &QTimer::timeout, this, [this](){
        if(countCam3 < 61)
            countCam3++;
        if(countCam3 == 5)
            ui->lblCam3->clear();
        if(countCam3 == 60){
            ui->cbCam3->setChecked(false);
        }
    });

    QStringList args;

    procRead = new QProcess(this);
    procRead->setProgram("ffmpeg");
    connect(procRead, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            [=](int exitCode, QProcess::ExitStatus exitStatus){
                qDebug()<<"exitCodeRead = "<<exitCode<<"exitStatusRead = "<<exitStatus;
                countCam1 = 0;
                countCam2 = 0;
                countCam3 = 0;
                startReadCams();
            });
    connect(procRead, &QProcess::started, this, [this](){
        if(camToVk > 0 && procVk->state() == QProcess::NotRunning){
            if(camToVk == 1 && ui->cbCam1->isChecked())
                onStreamVk();
            if(camToVk == 2 && ui->cbCam2->isChecked())
                onStreamVk();
            if(camToVk == 3 && ui->cbCam3->isChecked())
                onStreamVk();
        }
    });
    connect(procRead, &QProcess::readyReadStandardError, this, [this](){
        QByteArray ba = procRead->readAllStandardError();
        qDebug()<<"err read = "<<ba;

        if(ba.contains("I/O error") || ba.contains("Unknown error")){
            QFile file("read_error.txt");
            if (!file.open(QIODevice::Append | QIODevice::Text))
                return;
            file.write(ba, ba.length());
            file.close();

            if((ba.contains(urlWebCam1.toUtf8()) &&  urlWebCam1 != "") || (ba.contains(urlWebCam2.toUtf8()) && urlWebCam2 != "")){
                stopRecord(false);
                ui->cbCam1->setChecked(false);
                ui->cbCam2->setChecked(false);
                ui->cbCam3->setChecked(false);
                killFfmpegProcess();
                return;
            }
            if(ba.contains(urlCam1.toUtf8()) &&  urlCam1 != ""){
                ui->cbCam1->setChecked(false);
                stopRecord(false);
                stopReadCams();
                return;
            }
            if(ba.contains(urlCam2.toUtf8()) &&  urlCam2 != ""){
                ui->cbCam2->setChecked(false);
                stopRecord(false);
                stopReadCams();
                return;
            }
            if(ba.contains(urlCam3.toUtf8()) &&  urlCam3 != ""){
                ui->cbCam3->setChecked(false);
                stopRecord(false);
                stopReadCams();
                return;
            }
        }
    });

    procReadCam1 = new QProcess(this);
    procReadCam1->setProgram("ffmpeg");
    args<<"-i"<<"udp://127.0.0.1:5001"<<"-filter_complex"<<"[0:v]scale=100:50, fps=1"<<"-vcodec"<<"png"<<"-f"<<"image2pipe"<<"-";
    procReadCam1->setArguments(args);
    connect(procReadCam1, &QProcess::readyReadStandardOutput, this, [this](){
        QByteArray ba = procReadCam1->readAllStandardOutput();
        QImage image(100, 50, QImage:: Format_RGB666);
        if (image.loadFromData(ba, "PNG"))
            ui->lblCam1->setPixmap(QPixmap::fromImage(image));
        countCam1 = 0;
    });

    procReadCam2 = new QProcess(this);
    procReadCam2->setProgram("ffmpeg");
    args.clear();
    args<<"-i"<<"udp://127.0.0.1:5002"<<"-filter_complex"<<"[0:v]scale=100:50, fps=2 "<<"-vcodec"<<"png"<<"-f"<<"image2pipe"<<"-";
    procReadCam2->setArguments(args);
    connect(procReadCam2, &QProcess::readyReadStandardOutput, this, [this](){
        QByteArray ba = procReadCam2->readAllStandardOutput();
        QImage image(100, 50, QImage:: Format_RGB666);
        if (image.loadFromData(ba, "PNG"))
            ui->lblCam2->setPixmap(QPixmap::fromImage(image));
        countCam2 = 0;
    });

    procReadCam3 = new QProcess(this);
    procReadCam3->setProgram("ffmpeg");
    args.clear();
    args<<"-i"<<"udp://127.0.0.1:5003"<<"-filter_complex"<<"[0:v]scale=100:50, fps=2"<<"-vcodec"<<"png"<<"-f"<<"image2pipe"<<"-";
    procReadCam3->setArguments(args);
    connect(procReadCam3, &QProcess::readyReadStandardOutput, this, [this](){
        QByteArray ba = procReadCam3->readAllStandardOutput();
        QImage image(100, 50, QImage:: Format_RGB666);
        if (image.loadFromData(ba, "PNG"))
            ui->lblCam3->setPixmap(QPixmap::fromImage(image));
        countCam3 = 0;
    });

    procRecord = new QProcess(this);
    procRecord->setProgram("ffmpeg");
    connect(procRecord, &QProcess::readyReadStandardError, this, [this](){
        QByteArray ba = procRecord->readAllStandardError();
        qDebug()<<"err rec ="<<ba;

        if(!ba.contains("I/O error"))
            return;
        QFile file("record.txt");
        if (!file.open(QIODevice::Append | QIODevice::Text))
            return;
        file.write(ba, ba.length());
        file.close();

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
        qDebug()<<s;
        if(s.contains("I/O error")){
            QFile file("vk.txt");
            if (!file.open(QIODevice::Append | QIODevice::Text))
                return;
            file.write(s, s.length());
            file.close();
            procVk->write("q");
        }
    });
    connect(procVk, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            [=](int exitCode, QProcess::ExitStatus exitStatus){
                qDebug()<<"vk exitCode = "<<exitCode<<"vk exitStatus = "<<exitStatus;
                //stopReadCams();
                ui->labelvk->setStatusVk(false);
            });
    connect(procVk, &QProcess::started, this, [=](){
        lastCountFrame = 0;
        ui->labelvk->setStatusVk(true);
    });

    connect(btnPlay, &QPushButton::clicked, this, [this](){
        stopRecord();
        QString file = QFileDialog::getOpenFileName(nullptr, "Выбор видео", "videos");
        if(file == "" || !(file.endsWith(".mp4")
                            || file.endsWith(".mkv")
                            || file.endsWith(".mov")
                            || file.endsWith(".avi")))
            return;
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

        player->showFullScreen();
        player->setMediaUrl("videos/" + dirList.at(0));
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
    });

    //////////////////////////////////////////////
    /// %1 - отступ плашки, %2 - высота плашки ///
    ////////////////////////////////////////////// 
    filterStreamVk = "drawbox=%1:ih-%2-3*%1:(iw-%1*10)*0.45+%1*3:%2+2*%1:blue@0.5:t=fill, "
                     "drawbox=%1*2 :ih-%2-2*%1: (iw-%1*10)*0.35: %2:black:t=fill, "
                     "drawtext=fontsize=%2/2.5:fontcolor=White:fontfile=arial.ttf:textfile=name_blue.txt:x=%1*2+(%2-th)/2: y=h-%2-2*%1+(%2-th)/2: reload=1: line_spacing=5, "
                     "drawbox=%1*3+(iw-%1*10)*0.35: ih-%2-2*%1: (iw-%1*10)/10: %2:black:t=fill, "
                     "drawbox=%1*2+(iw-%1*4)*4.5/10: ih-%2-2*%1: (iw-%1*4)/10: %2:black:t=fill, "
                     "drawtext=fontsize=%2*4/5:fontcolor=White:fontfile=arial.ttf:textfile=rate_blue.txt:x=%1*3+(w-%1*10)*0.35+((w-%1*10)/10-tw)/2: y=h-%2-2*%1+(%2-th)/2: reload=1: shadowcolor=blue:shadowx=4: shadowy=4, "
                     "drawtext=fontsize=%2*4/5:fontcolor=LightGreen:fontfile=arial.ttf:textfile=time.txt:x=%1*2+(w-%1*4)*4.5/10+((w-%1*4)/10-tw)/2 :y=h-%2-2*%1+(%2-th)/2 :reload=1, "
                     "drawbox=iw-(iw-%1*10)*0.45-%1*3-%1: ih-%2-3*%1: (iw-%1*10)*0.45+%1*3: %2+2*%1:red@0.5:t=fill, "
                     "drawbox=iw-(iw-%1*10)*0.45-%1*3: ih-%2-2*%1: (iw-%1*10)/10: %2: black:t=fill, "
                     "drawtext=fontsize=%2*4/5:fontcolor=white:fontfile=arial.ttf:textfile=rate_red.txt: x=w-(w-%1*10)*0.45-%1*3+((w-%1*10)/10-tw)/2 :y=h-%2-2*%1+(%2-th)/2 :reload=1: shadowcolor=red: shadowx=4: shadowy=4, "
                     "drawbox=iw-w-%1*2 :ih-%2-2*%1: (iw-%1*10)*0.35: %2:black:t=fill, "
                     "drawtext=fontsize=%2/2.5:fontcolor=white:fontfile=arial.ttf:textfile=name_red.txt: x=w-(w-%1*10)*0.35-%1*2+(%2-th)/2: y=h-%2-2*%1+(%2-th)/2: reload=1: line_spacing=5";

    videoSettings = new SettingsVideoReplay(this);
    connect(videoSettings, &SettingsVideoReplay::sigShowReplayOnTv, this, &VideoReplayControl::sigShowReplayOnTv);

}

VideoReplayControl::~VideoReplayControl()
{
    killFfmpegProcess();
    delete player;
    delete ui;
}

void VideoReplayControl::setPlayerTv(PlayerTv *p)
{
    player->setTvPlayer(p);
}

void VideoReplayControl::killFfmpegProcess()
{
    ui->cbCam1->setChecked(false);
    ui->cbCam2->setChecked(false);
    ui->cbCam3->setChecked(false);
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
    if(procVk->state() == QProcess::Running){
        procVk->write("q");
        procVk->waitForBytesWritten();
        procVk->closeWriteChannel();
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
        QStringList args({"-y", "-loglevel", "error", "-c:v", each, "-i", "test.mov", "-frames:v", "1", "test.mp4"});
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
    procReadCam1->start();
    procReadCam2->start();
    procReadCam3->start();

    int isCam1 = ui->cbCam1->isChecked() ? 1 : 0;
    int isCam2 = ui->cbCam2->isChecked() ? 2 : 0;
    int isCam3 = ui->cbCam3->isChecked() ? 4 : 0;

    QStringList argsInput1;
    QStringList argsInput2;
    QStringList argsInput3;

    int turnOnCams = isCam1 + isCam2 + isCam3;
    if(turnOnCams == 0)
        return;

    QString filter = filterStreamVk.arg(20).arg(heightPipVk);

    QString codec = hwEncoder == "" ? "mpeg2video" : hwEncoder;
    if(hwDecoder == "")
        codec = "mpeg2video";

    QStringList args;

    args<<"-hide_banner"<<"-loglevel"<<"error";

    if(urlWebCam1 != ""){
        if(hwDecoder != "")
            argsInput1<<"-c:v"<<hwDecoder;
        QString url = "video=" + urlWebCam1;
        if(urlSound != "")
           url += ":audio=" + urlSound;
        argsInput1<<"-f"<<"dshow"<<"-rtbufsize"<<"10M"<<"-framerate"<<fps1<<"-video_size"<<resolution1<<"-i"<<url;
    }
    else{
        argsInput1<<"-rtbufsize"<<"10M";
        if(urlCam1.startsWith("rtsp"))
            argsInput1<<"-rtsp_transport"<<"tcp"<<"-stimeout"<<"1000000";
        argsInput1<<"-i"<<urlCam1;
    }

    if(urlWebCam2 != ""){
        if(hwDecoder != "")
            argsInput2<<"-c:v"<<hwDecoder;
        QString url = "video=" + urlWebCam2;
        if(urlSound != "")
            url += ":audio=" + urlSound;
        argsInput2<<"-f"<<"dshow"<<"-rtbufsize"<<"10M"<<"-framerate"<<fps2<<"-video_size"<<resolution2<<"-i"<<url;//<<"-pix_fmt"<<"yuv420p";
    }
    else{
        argsInput2<<"-rtbufsize"<<"10M";
        if(urlCam2.startsWith("rtsp"))
            argsInput2<<"-rtsp_transport"<<"tcp"<<"-stimeout"<<"1000000";
        argsInput2<<"-i"<<urlCam2;
    }

    argsInput3<<"-rtbufsize"<<"10M";
    if(urlCam3.startsWith("rtsp"))
        argsInput3<<"-rtsp_transport"<<"tcp"<<"-stimeout"<<"1000000";
    argsInput3<<"-i"<<urlCam3;

    QStringList argsAudio;

    if(camToVk > 0){
        if(urlSound != "")
            argsAudio<<"-rtbufsize"<<"10M"<<"-f"<<"dshow"<<"-i"<<"audio=" + urlSound;
        else{
            QMessageBox msgBox;
            msgBox.setText("Нет источника звука. Трансляция невозможна");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
            ui->cbCam1->setChecked(false);
            ui->cbCam2->setChecked(false);
            ui->cbCam3->setChecked(false);
            return;
        }
    }

    switch(turnOnCams){
    case 1:             //1
        args += argsInput1;
        if(camToVk > 0)
            args += argsAudio;
        if(urlWebCam1 != ""){
            args<<"-map"<<"0"<<"-g"<<"10"<<"-c:v"<<codec<<"-b:v"<<"3M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5000";
            args<<"-map"<<"0:v"<<"-c:v"<<codec<<"-r"<<"5"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5001";
        }
        else{
            args<<"-map"<<"0"<<"-g"<<"10"<<"-c:v"<<"copy"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5000";
            args<<"-map"<<"0:v"<<"-c:v"<<"copy"<<"-r"<<"5"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5001";
        }
        if(camToVk == 1){
            args<<"-map"<<"0:v"<<"-map"<<"1:a"<<"-vf"<<filter<<"-b:v"<<"3M";
            if(urlWebCam1 != "")
                args<<"-c:v"<<codec<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
            else{
                if(hwEncoder != "")
                    args<<"-c:v"<<hwEncoder;
                args<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
            }
        }
        break;
    case 2:             //2
        args += argsInput2;
        if(camToVk > 0)
            args += argsAudio;
        if(urlWebCam2 != ""){
            args<<"-map"<<"0"<<"-g"<<"10"<<"-c:v"<<codec<<"-b:v"<<"3M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5000";
            args<<"-map"<<"0:v"<<"-c:v"<<codec<<"-r"<<"5"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5002";
        }
        else{
            args<<"-map"<<"0"<<"-g"<<"10"<<"-c:v"<<"copy"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5000";
            args<<"-map"<<"0:v"<<"-c:v"<<"copy"<<"-r"<<"5"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5002";
        }
        if(camToVk == 2){
            args<<"-map"<<"0:v"<<"-map"<<"1:a"<<"-vf"<<filter<<"-b:v"<<"3M";
            if(urlWebCam2 != "")
                args<<"-c:v"<<codec<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
            else{
                if(hwEncoder != "")
                    args<<"-c:v"<<hwEncoder;
                args<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
            }
        }
        break;
    case 4:             //3   
        args += argsInput3;
        if(camToVk > 0)
            args += argsAudio;
        args<<"-map"<<"0"<<"-g"<<"10"<<"-vcodec"<<"copy"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5000";
        args<<"-map"<<"0:v"<<"-vcodec"<<"copy"<<"-r"<<"5"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5003";
        if(camToVk == 3){
            args<<"-map"<<"0:v"<<"-map"<<"1:a"<<"-vf"<<filter<<"-b:v"<<"3M";
            if(hwEncoder != "")
                args<<"-c:v"<<hwEncoder;
            args<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
        }
        break;
    case 3:             //1 & 2
        args += argsInput1;
        args += argsInput2;
        if(camToVk > 0)
            args += argsAudio;
        args<<"-map"<<"0";
        args<<"-map"<<"1";
        args<<"-g"<<"10"<<"-c:v:0";
        if(urlWebCam1 != "") args<<codec;
        else args<<"copy";
        args<<"-c:v:1";
        if(urlWebCam2 != "") args<<codec;
        else args<<"copy";
        args<<"-b:v"<<"3M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5000";
        args<<"-map"<<"0:v"<<"-c:v";
        if(urlWebCam1 != "") args<<codec;
        else args<<"copy";
        args<<"-r"<<"5"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5001";
        args<<"-map"<<"1:v"<<"-c:v";
        if(urlWebCam2 != "") args<<codec;
        else args<<"copy";
        args<<"-r"<<"5"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5002";
        if(camToVk == 1){
            args<<"-map"<<"0:v"<<"-map"<<"2:a"<<"-vf"<<filter<<"-b:v"<<"3M";
            if(urlWebCam1 != "")
                args<<"-c:v"<<codec<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
            else{
                if(hwEncoder != "")
                    args<<"-c:v"<<hwEncoder;
                args<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
            }
        }
        else if(camToVk == 2){
            args<<"-map"<<"1:v"<<"-map"<<"2:a"<<"-vf"<<filter<<"-b:v"<<"3M";
            if(urlWebCam2 != "")
                args<<"-c:v"<<codec<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
            else{
                if(hwEncoder != "")
                    args<<"-c:v"<<hwEncoder;
                args<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
            }
        }
        break;
    case 5:             //1 & 3
        args += argsInput1;
        args += argsInput3;
        if(camToVk > 0)
            args += argsAudio;
        args<<"-map"<<"0";
        args<<"-map"<<"1";
        args<<"-g"<<"10"<<"-c:v:0";
        if(urlWebCam1 != "") args<<codec;
        else args<<"copy";
        args<<"-c:v:1"<<"copy";
        args<<"-b:v"<<"3M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5000";
        args<<"-map"<<"0:v"<<"-c:v";
        if(urlWebCam1 != "") args<<codec;
        else args<<"copy";
        args<<"-r"<<"5"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5001";
        args<<"-map"<<"1:v"<<"-vcodec"<<"copy"<<"-r"<<"5"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5003";    
        if(camToVk == 1){
            args<<"-map"<<"0:v"<<"-map"<<"2:a"<<"-vf"<<filter<<"-b:v"<<"3M";
            if(hwEncoder != "")
                args<<"-c:v"<<hwEncoder;
            args<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
        }
        else if(camToVk == 3){
            args<<"-map"<<"1:v"<<"-map"<<"2:a"<<"-vf"<<filter<<"-b:v"<<"3M";
            if(hwEncoder != "")
                args<<"-c:v"<<hwEncoder;
            args<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
        }
        break;
    case 6:             //2 & 3
        args += argsInput2;
        args += argsInput3;
        if(camToVk > 0)
            args += argsAudio;
        args<<"-map"<<"0";
        args<<"-map"<<"1";
        args<<"-g"<<"10"<<"-c:v:0";
        if(urlWebCam2 != "") args<<codec;
        else args<<"copy";
        args<<"-c:v:1"<<"copy";
        args<<"-b:v"<<"3M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5000";
        args<<"-map"<<"0:v"<<"-c:v";
        if(urlWebCam2 != "") args<<codec;
        else args<<"copy";
        args<<"-r"<<"5"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5002";
        args<<"-map"<<"1:v"<<"-vcodec"<<"copy"<<"-r"<<"5"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5003";
        if(camToVk == 2){
            args<<"-map"<<"0:v"<<"-map"<<"2:a"<<"-vf"<<filter<<"-b:v"<<"3M";
            if(hwEncoder != "")
                args<<"-c:v"<<hwEncoder;
            args<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
        }
        else if(camToVk == 3){
            args<<"-map"<<"1:v"<<"-map"<<"2:a"<<"-vf"<<filter<<"-b:v"<<"3M";
            if(hwEncoder != "")
                args<<"-c:v"<<hwEncoder;
            args<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
        }
        break;
    case 7:             //1 & 2 & 3
        args += argsInput1;
        args += argsInput2;
        args += argsInput3;
        if(camToVk > 0)
            args += argsAudio;
        args<<"-map"<<"0";
        args<<"-map"<<"1";
        args<<"-map"<<"2";
        args<<"-g"<<"10"<<"-c:v:0";
        if(urlWebCam1 != "") args<<codec;
        else args<<"copy";
        args<<"-c:v:1";
        if(urlWebCam2 != "") args<<codec;
        else args<<"copy";
        args<<"-c:v:2"<<"copy";
        args<<"-b:v"<<"3M"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5000";
        args<<"-map"<<"0:v"<<"-c:v";
        if(urlWebCam1 != "") args<<codec;
        else args<<"copy";
        args<<"-r"<<"5"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5001";
        args<<"-map"<<"1:v"<<"-c:v";
        if(urlWebCam2 != "") args<<codec;
        else args<<"copy";
        args<<"-r"<<"5"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5002";
        args<<"-map"<<"2:v"<<"-vcodec"<<"copy"<<"-r"<<"5"<<"-f"<<"mpegts"<<"udp://127.0.0.1:5003";
        if(camToVk == 1){
            args<<"-map"<<"0:v"<<"-map"<<"3:a"<<"-vf"<<filter<<"-b:v"<<"3M";
            if(hwEncoder != "")
                args<<"-c:v"<<hwEncoder;
            args<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
        }
        else if(camToVk == 2){
            args<<"-map"<<"1:v"<<"-map"<<"3:a"<<"-vf"<<filter<<"-b:v"<<"3M";
            if(hwEncoder != "")
                args<<"-c:v"<<hwEncoder;
            args<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
        }
        else if(camToVk == 3){
            args<<"-map"<<"2:v"<<"-map"<<"3:a"<<"-vf"<<filter<<"-b:v"<<"3M";
            if(hwEncoder != "")
                args<<"-c:v"<<hwEncoder;
            args<<"-f"<<"mpegts"<<"udp://127.0.0.1:5004";
        }
        break;
    }



    procRead->setArguments(args);
    procRead->start();
    qDebug()<<"start procRead"<<args;

}

void VideoReplayControl::stopReadCams()
{
    qDebug()<<"stopReadCams";
    procRead->write("q");
    procRead->closeWriteChannel();
}

void VideoReplayControl::onStreamVk()
{
    QStringList args;
    args<<"-hide_banner"<<"-timeout"<<"20000000";
    args<<"-thread_queue_size"<<"1024";
    args<<"-i"<<"udp://127.0.0.1:5004"; //0
    args<<"-c:v"<<"copy"<<"-f"<<"flv"<<urlVk + keyVk;
    qDebug()<<"vk = "<<args;
    procVk->setArguments(args);
    procVk->start();
    //}
}

void VideoReplayControl::setWebCam1(QString cam)
{
    urlWebCam1 = cam;
}

void VideoReplayControl::setWebCam2(QString cam)
{
    urlWebCam2 = cam;
}

void VideoReplayControl::setParamWebCam1(QList<int> param)
{
    fps1 = QString::number(param.at(0));
    resolution1 = QString::number(param.at(1)) + "x" + QString::number(param.at(2));
}

void VideoReplayControl::setParamWebCam2(QList<int> param)
{
    fps2 = QString::number(param.at(0));
    resolution2 = QString::number(param.at(1)) + "x" + QString::number(param.at(2));
}

void VideoReplayControl::setSound(QString sound)
{
    urlSound = sound;
}

void VideoReplayControl::setCam1(QString cam)
{
    urlCam1 = cam;
    qDebug()<<"urlCam1 = "<<urlCam1;
}

void VideoReplayControl::setCam2(QString cam)
{
    urlCam2 = cam;
    qDebug()<<"urlCam2 = "<<urlCam2;
}

void VideoReplayControl::setCam3(QString cam)
{
    urlCam3 = cam;
    qDebug()<<"urlCam3 = "<<urlCam3;
}

void VideoReplayControl::startRecord(bool b, QString s)
{
    btnPlay->setEnabled(!b);
    btnPlayLast->setEnabled(!b);

    if(procRead->state() == QProcess::NotRunning)
        return;

    if(procRecord->state() == QProcess::Starting || procRecord->state() == QProcess::Running || !b)
        return;

    QString file = "videos/" + s + "_" + QTime::currentTime().toString("hh:mm:ss");
    file.replace(":", "_");

    QStringList args;
    args<<"-hide_banner"<<"-loglevel"<<"error"<<"-i"<<"udp://127.0.0.1:5000"<<"-map"<<"0"<<"-c:v"<<"copy"<<"-c:a"<<"aac"<<file + ".mp4";
    qDebug()<<args;
    procRecord->setArguments(args);
    procRecord->start();
}

void VideoReplayControl::stopRecord(bool b)
{
    btnPlay->setEnabled(true);
    btnPlayLast->setEnabled(true);
    if(procRecord->state() == QProcess::Running){
        procRecord->write("q");
        procRecord->waitForBytesWritten();
        procRecord->closeWriteChannel();
        if(b)
            procRecord->waitForFinished();
    }
}

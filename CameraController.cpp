#include "CameraController.h"
#include "qdatetime.h"
#include "qdir.h"
#include "qsettings.h"
#include "ui_CameraController.h"
#include <QFileDialog>

CameraController::CameraController(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    setAutoFillBackground(true);

    player = new PlayerPc;

    firstVideoPacket3.data = QByteArray();

    cam1 = new UsbCameraWidget;
    cam2 = new UsbCameraWidget;
    cam3 = new UsbCameraWidget;
    ui->layoutCams->addWidget(cam1);
    ui->layoutCams->addWidget(cam2);
    ui->layoutCams->addWidget(cam3);
    cam1->setCameraNumber(1);
    cam2->setCameraNumber(2);
    cam3->setCameraNumber(3);

    //cam3->setStream("rtmp://ovsu.okcdn.ru/input/14549276306204_16031179803164_j4pwhty6vu");

    btnPlay = new SvgButton(":/images/play_choice_enable.svg", ":/images/play_choice_disable.svg", this);
    btnPlay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->layoutButtons->insertWidget(0, btnPlay);
    btnPlay->setMinimumHeight(50);

    btnPlayLast = new SvgButton(":/images/play_last_enable.svg", ":/images/play_last_disable.svg", this);
    btnPlayLast->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->layoutButtons->insertWidget(1, btnPlayLast);

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

    QString fileSettings = "settings.ini";

    settings = new QSettings(fileSettings, QSettings::IniFormat, this);
    //settings.setPath(QSettings::IniFormat, QSettings::UserScope, fileSettings);
    settings->beginGroup("vk");
    urlStream = settings->value("url", "").toString();
    keyStream = settings->value("key", "").toString();
    streamCam = settings->value("cam", 0).toInt();
    settings->endGroup();

    settings->beginGroup("hwDecoder");
    QString hwDecoder = settings->value("decoder", "нет").toString();
    settings->endGroup();

    cam1->setHwDecoder(hwDecoder);
    cam2->setHwDecoder(hwDecoder);
    cam3->setHwDecoder(hwDecoder);

    ui->cmbHwDecoder->insertItems(0, {"нет", "cuda", "dxva2", "d3d11va", "d3d12va", "vulkan"});
    ui->cmbHwDecoder->setCurrentText(hwDecoder);
    connect(ui->cmbHwDecoder, &QComboBox::currentTextChanged, this, [this](const QString s){
        settings->beginGroup("hwDecoder");
        settings->setValue("decoder", s);
        settings->endGroup();
        cam1->setHwDecoder(s);
        cam2->setHwDecoder(s);
        cam3->setHwDecoder(s);
    });

    cameras[1] = "";
    cameras[2] = "";
    cameras[3] = "";

    threadRecorder = new QThread(this);

    connect(cam1, &UsbCameraWidget::sigVideoPacket, this, &CameraController::slotVideo1);
    connect(cam2, &UsbCameraWidget::sigVideoPacket, this, &CameraController::slotVideo2);
    connect(cam3, &UsbCameraWidget::sigVideoPacket, this, &CameraController::slotVideo3);
    connect(cam1, &UsbCameraWidget::sigSoundPacket, this, &CameraController::slotSound);
    connect(cam2, &UsbCameraWidget::sigSoundPacket, this, &CameraController::slotSound);
    connect(cam3, &UsbCameraWidget::sigSoundPacket, this, &CameraController::slotSound);

    connect(cam1, &UsbCameraWidget::sigParams, this, &CameraController::slotParams);
    connect(cam2, &UsbCameraWidget::sigParams, this, &CameraController::slotParams);
    connect(cam3, &UsbCameraWidget::sigParams, this, &CameraController::slotParams);

    procFindConnectedPhones.setProgram("platform-tools/adb");
    procSetupPorts.setProgram("platform-tools/adb");

    connect(&procSetupPorts, &QProcess::readyReadStandardOutput, this, [this](){
        qDebug()<<"procSetupPorts readyReadStandardOutput";
        QString output = procSetupPorts.readAllStandardOutput();
        qDebug()<<output;
    });


    connect(&procFindConnectedPhones, &QProcess::readyReadStandardOutput, this, [this](){
        qDebug()<<"procFindConnectedPhones readyReadStandardOutput";
        QString output = procFindConnectedPhones.readAllStandardOutput();
        QStringList list = output.split("\n");
        devices.clear();
        if(!list.at(0).contains("List of devices attached") || list.count() < 2)
            return;
        for(int i = 1; i < list.count(); i++){
            if(list.at(i).contains("device")){
                devices.append(list.at(i).split("\t").at(0));
            }
        }
        qDebug()<<devices<<list;
        foreach(auto each, devices){
            QStringList list = getCamParameters(each);
            if(list.isEmpty()) continue;
            qDebug()<<list;
            if(list.at(0) == "1"){
                // cam1Parameters.isConnect = true;
                // cam1Parameters.width = list.at(1).toInt();
                // cam1Parameters.height = list.at(2).toInt();
                // cam1Parameters.fps = list.at(3).toInt();
                // cam1Parameters.device = each;
                cameras.insert(1, each);
            }
            else if(list.at(0) == "2"){
                // cam2Parameters.isConnect = true;
                // cam2Parameters.width = list.at(1).toInt();
                // cam2Parameters.height = list.at(2).toInt();
                // cam2Parameters.fps = list.at(3).toInt();
                // cam2Parameters.device = each;
                cameras.insert(2, each);
            }
            else {
                // cam3Parameters.isConnect = true;
                // cam3Parameters.width = list.at(1).toInt();
                // cam3Parameters.height = list.at(2).toInt();
                // cam3Parameters.fps = list.at(3).toInt();
                // cam3Parameters.device = each;
                cameras.insert(3, each);
            }
        }
        qDebug()<<cameras<<list;

        // configure();
        //startRecord();
    });
    connect(&procFindConnectedPhones, &QProcess::finished, this, [this](){
        qDebug()<<"err"<<procFindConnectedPhones.readAllStandardError();
        if(ui->cbTurnCams->isChecked()){
            qDebug()<<"setup ports 0";
            if(cameras[1] != ""){
                procSetupPorts.setArguments({"-s", cameras[1], "reverse", "tcp:5551", "tcp:5551"});
                procSetupPorts.start();
                procSetupPorts.waitForFinished();
                procSetupPorts.setArguments({"-s", cameras[1], "reverse", "tcp:5552", "tcp:5552"});
                procSetupPorts.start();
                procSetupPorts.waitForFinished();
                procSetupPorts.setArguments({"-s", cameras[1], "forward", "tcp:5553", "tcp:5553"});
                procSetupPorts.start();
                procSetupPorts.waitForFinished();
                //led1->checkConnection();
                qDebug()<<"setup ports 1";
                cam1->setSound(true);
                cam1->startCamera();
                qDebug()<<"setup ports 2";
                isCam1 = true;
            }
            if(cameras[2] != ""){
                procSetupPorts.setArguments({"-s", cameras[2], "reverse", "tcp:5561", "tcp:5561"});
                procSetupPorts.start();
                procSetupPorts.waitForFinished();
                procSetupPorts.setArguments({"-s", cameras[2], "reverse", "tcp:5562", "tcp:5562"});
                procSetupPorts.start();
                procSetupPorts.waitForFinished();
                procSetupPorts.setArguments({"-s", cameras[2], "forward", "tcp:5563", "tcp:5563"});
                procSetupPorts.start();
                procSetupPorts.waitForFinished();
                //led2->checkConnection();
                if(!isCam1)
                    cam2->setSound(true);
                cam2->startCamera();
                isCam2 = true;
            }
            if(cameras[3] != ""){
                procSetupPorts.setArguments({"-s", cameras[3], "reverse", "tcp:5571", "tcp:5571"});
                procSetupPorts.start();
                procSetupPorts.waitForFinished();
                procSetupPorts.setArguments({"-s", cameras[3], "reverse", "tcp:5572", "tcp:5572"});
                procSetupPorts.start();
                procSetupPorts.waitForFinished();
                procSetupPorts.setArguments({"-s", cameras[3], "forward", "tcp:5573", "tcp:5573"});
                procSetupPorts.start();
                procSetupPorts.waitForFinished();
                //led3->checkConnection();
                qDebug()<<"setup ports 1";
                if(!(isCam1 || isCam2))
                    cam3->setSound(true);
                cam3->startCamera();
                qDebug()<<"setup ports 2";
                isCam3 = true;
            }
        }
        else{
            cameras[1] = "";
            cam1->stopCamera();
            cameras[2] = "";
            cam2->stopCamera();
            cameras[3] = "";
            cam3->stopCamera();
            isSound = false;
            isCam1= false;
            isCam2 = false;
            isCam3 = false;
            // startIdStream1 = -1;
            // startIdStream2 = -1;
            // startIdStream3 = -1;
            // if(decoderContext1){
            //     avcodec_free_context(&decoderContext1);
            //     decoderContext1 = NULL;
            // }
            // if(decoderContext2){
            //     avcodec_free_context(&decoderContext2);
            //     decoderContext2 = NULL;
            // }
            // if(decoderContext3){
            //     avcodec_free_context(&decoderContext3);
            //     decoderContext3 = NULL;
            // }
            // if(oc){
            //     avformat_free_context(oc);
            //     oc = NULL;
            // }
            // if(contextVk){
            //     avformat_free_context(contextVk);
            //     contextVk = NULL;
            // }
        }
    });

    //procFindConnectedPhones.setProgram("platform-tools/adb");

    connect(ui->cbTurnCams, &QCheckBox::clicked, this, [this](bool state){
        ui->cbTurnCams->setEnabled(false);
        QTimer::singleShot(5000, this, [this](){ui->cbTurnCams->setEnabled(true);});
        if(state){
            procFindConnectedPhones.setArguments({"devices"});
        }else{
            procFindConnectedPhones.setArguments({"kill-server"});
            // if(firstVideo1Packet != NULL)
            //     av_packet_free(&firstVideo1Packet);
            // if(firstVideo2Packet != NULL)
            //     av_packet_free(&firstVideo2Packet);
            // if(firstVideo3Packet != NULL)
            //     av_packet_free(&firstVideo3Packet);
        }
        procFindConnectedPhones.start();
        firstVideoPacket1.data = QByteArray();
        firstVideoPacket2.data = QByteArray();
        firstVideoPacket3.data = QByteArray();
        qDebug()<<"procFindConnectedPhones.start()";
    });
    procFindConnectedPhones.setArguments({"kill-server"});
    procFindConnectedPhones.start();
}

CameraController::~CameraController()
{
    qDebug()<<"~Widget()";
    delete player;
    cam1->stopCamera();
    cam2->stopCamera();
    cam3->stopCamera();
    // cam1->deleteLater();
    // cam2->deleteLater();
    // cam3->deleteLater();
    delete ui;
}

QStringList CameraController::getCamParameters(QString device)
{
    procGetCamParameters.setProgram("platform-tools/adb");
    procGetCamParameters.setArguments({"-s", device, "forward", "tcp:6666", "tcp:6666"});
    procGetCamParameters.start();
    procGetCamParameters.waitForFinished(500);
    procGetCamParameters.close();
    QTcpSocket socket;
    socket.connectToHost(QHostAddress::LocalHost, 6666);
    socket.waitForConnected(500);
    socket.waitForReadyRead(500);
    QByteArray ba = socket.readAll();
    socket.disconnectFromHost();
    socket.close();
    QString str = QString::fromUtf8(ba);
    QStringList list = str.split('=');
    if(list.count() == 4)
        return list;
    else
        return QStringList();
}

void CameraController::slotVideo1(packet p){
    if(firstVideoPacket1.data.size() == 0)
        firstVideoPacket1 = p;
    emit sigPacket(p, 1);
}

void CameraController::slotVideo2(packet p){
    if(firstVideoPacket2.data.size() == 0)
        firstVideoPacket2 = p;
    emit sigPacket(p, 2);
}

void CameraController::slotVideo3(packet p){
    if(firstVideoPacket3.data.size() == 0)
        firstVideoPacket3 = p;
    emit sigPacket(p, 3);
}

void CameraController::slotSound(packet p){
    emit sigPacket(p, 0);
}

void CameraController::slotParams(int c, int w, int h, int f){
    params.insert(c, QList({w, h, f}));
}

void CameraController::startRecord(bool b, QString s){
    btnPlay->setEnabled(!b);
    btnPlayLast->setEnabled(!b);

    // if(procRead->state() == QProcess::NotRunning)
    //     return;

    // if(procRecord->state() == QProcess::Starting || procRecord->state() == QProcess::Running || !b)
    //     return;
    if(threadRecorder->isRunning()) return;
    qDebug()<<"startRecord";
    QString file = "videos/" + s + "_" + QTime::currentTime().toString("hh:mm:ss") + ".mp4";
    file.replace(":", "_");

    // QStringList args;
    // args<<"-hide_banner"<<"-loglevel"<<"error"<<"-i"<<"udp://127.0.0.1:5000"<<"-map"<<"0"<<"-c:v"<<"copy"<<"-c:a"<<"aac"<<file + ".mp4";
    // qDebug()<<args;
    // procRecord->setArguments(args);
    // procRecord->start();



    if(isCam1 || isCam2 || isCam3){
        int countCams = 0;
        if(isCam1) countCams++;
        if(isCam2) countCams++;
        if(isCam3) countCams++;

        recorder = new RecordWorker(file, params, {isCam1 == true ? &firstVideoPacket1 : NULL,
                                                   isCam2 == true ? &firstVideoPacket2 : NULL,
                                                   isCam3 == true ? &firstVideoPacket3 : NULL});
        connect(threadRecorder, &QThread::started, recorder, &RecordWorker::start);
        connect(threadRecorder, &QThread::finished, recorder, &RecordWorker::deleteLater);
        connect(this, &CameraController::sigPacket, recorder, &RecordWorker::packetHandler);
        recorder->moveToThread(threadRecorder);
        threadRecorder->start();
    }

}

void CameraController::stopRecord(){
    qDebug()<<"stopRecord";
    threadRecorder->quit();
}

void CameraController::setPlayerTv(PlayerTv *p)
{
    player->setTvPlayer(p);
}

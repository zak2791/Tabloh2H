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
    // cam1->setCameraNumber(1);
    // cam2->setCameraNumber(2);
    // cam3->setCameraNumber(3);

    btnPlay = new SvgButton(":/images/play_choice_enable.svg", ":/images/play_choice_disable.svg", this);
    btnPlay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->layoutButtons->insertWidget(0, btnPlay);
    btnPlay->setMinimumHeight(50);

    btnPlayLast = new SvgButton(":/images/play_last_enable.svg", ":/images/play_last_disable.svg", this);
    btnPlayLast->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->layoutButtons->insertWidget(1, btnPlayLast);

    //connect(&tmpSetupPorts, &QTimer::timeout, this, &CameraController::setupPorts);

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
    //keyStream = settings->value("key", "").toString();
    streamCam = settings->value("cam", 0).toInt();
    settings->endGroup();

    settings->beginGroup("hwDecoder");
    QString hwDecoder = settings->value("decoder", "нет").toString();
    settings->endGroup();

    settings->beginGroup("vk");
    urlStream = settings->value("url", "").toString();
    keyStream = settings->value("key", "").toString();
    streamCam = settings->value("cam", 0).toInt();
    //settings->endGroup();
    if(streamCam == 1)
        cam1->setStream(urlStream + keyStream);
    if(streamCam == 2)
        cam2->setStream(urlStream + keyStream);
    if(streamCam == 3)
        cam3->setStream(urlStream + keyStream);

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
    procSetupPorts1.setProgram("platform-tools/adb");
    procSetupPorts2.setProgram("platform-tools/adb");
    procSetupPorts3.setProgram("platform-tools/adb");
    procCheckDevices.setProgram("platform-tools/adb");
    procCheckDevices.setArguments({"devices"});

    connect(&procCheckDevices, &QProcess::finished, this, &CameraController::slotCheckDevices);

    /////////////////////////////////////////////////////////////////////////////////////
    ///                        Поиск подключенных смартфонов                          ///
    /////////////////////////////////////////////////////////////////////////////////////
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
        qDebug()<<"devices = "<<devices<<list;
        params.clear();
        foreach(auto each, devices){
            QStringList list = getCamParameters(each);
            if(list.isEmpty()) continue;
            qDebug()<<"list = "<<list;
            if(list.at(0) == "1"){
                cameras.insert(1, each);
                cam1->setCameraParameters(1, list.at(1).toInt(), list.at(2).toInt(), list.at(3).toInt());
                params.insert(1, QList({list.at(1).toInt(), list.at(2).toInt(), list.at(3).toInt()}));
            }
            else if(list.at(0) == "2"){
                cameras.insert(2, each);
                cam2->setCameraParameters(2, list.at(1).toInt(), list.at(2).toInt(), list.at(3).toInt());
                params.insert(2, QList({list.at(1).toInt(), list.at(2).toInt(), list.at(3).toInt()}));
            }
            else {
                cameras.insert(3, each);
                cam3->setCameraParameters(3, list.at(1).toInt(), list.at(2).toInt(), list.at(3).toInt());
                params.insert(3, QList({list.at(1).toInt(), list.at(2).toInt(), list.at(3).toInt()}));
            }
        }
        qDebug()<<"cameras = "<<cameras<<list<<"params ="<<params;

        connect(cam1, &UsbCameraWidget::sigCamOff, this, [this](){
            isCam1 = false;
            if(!(isCam1 || isCam2 || isCam3) && threadRecorder->isRunning())
                threadRecorder->quit();
        });
        connect(cam2, &UsbCameraWidget::sigCamOff, this, [this](){
            isCam2 = false;
            if(!(isCam1 || isCam2 || isCam3) && threadRecorder->isRunning())
                threadRecorder->quit();
        });
        connect(cam3, &UsbCameraWidget::sigCamOff, this, [this](){
            isCam3 = false;
            if(!(isCam1 || isCam2 || isCam3) && threadRecorder->isRunning())
                threadRecorder->quit();
        });

        // configure();
        //startRecord();
    });

    //////////////////////////////////////////////////////////////////////
    ///             Подключение найденных смартфонов                   ///
    //////////////////////////////////////////////////////////////////////
    connect(&procFindConnectedPhones, &QProcess::finished, this, [this](){
        qDebug()<<"err procFindConnectedPhones "<<procFindConnectedPhones.readAllStandardError();
        if(ui->cbTurnCams->isChecked()){
            qDebug()<<"setup ports 0";
            //setupPorts();
            if(cameras[1] != ""){
                cam1->setSound(true);
                cam1->startCamera(cameras[1]);
                qDebug()<<"setup ports 2";
                isCam1 = true;
            }
            if(cameras[2] != ""){
                if(!isCam1)
                    cam2->setSound(true);
                cam2->startCamera(cameras[2]);
                isCam2 = true;
            }
            if(cameras[3] != ""){
                qDebug()<<"setup ports 1";
                if(!(isCam1 || isCam2))
                    cam3->setSound(true);
                cam3->startCamera(cameras[3]);
                qDebug()<<"setup ports 2";
                isCam3 = true;
            }
            //procCheckDevices.start();
            //tmpSetupPorts.start(1000);
        }
        qDebug()<<"err procFindConnectedPhones 2";
        // else{
        //     //tmpSetupPorts.stop();
        //     cameras[1] = "";
        //     qDebug()<<"cam1->stopCamera()";
        //     cam1->stopCamera();
        //     cameras[2] = "";
        //     qDebug()<<"cam2->stopCamera()";
        //     cam2->stopCamera();
        //     cameras[3] = "";
        //     qDebug()<<"cam3->stopCamera()";
        //     cam3->stopCamera();
        //     isSound = false;
        //     isCam1= false;
        //     isCam2 = false;
        //     isCam3 = false;
        // }
    });

    //procFindConnectedPhones.setProgram("platform-tools/adb");

    connect(ui->cbTurnCams, &QCheckBox::clicked, this, [this](bool state){
        ui->cbTurnCams->setEnabled(false);
        QTimer::singleShot(5000, this, [this](){ui->cbTurnCams->setEnabled(true);});
        if(state){
            procFindConnectedPhones.setArguments({"devices"});
            //checkDevicesTimer.start(1000);
            firstVideoPacket1.data = QByteArray();
            firstVideoPacket2.data = QByteArray();
            firstVideoPacket3.data = QByteArray();
            procFindConnectedPhones.start();
        }
        else{
            // cam1->stopCamera();
            // cam2->stopCamera();
            // cam3->stopCamera();
            cameras[1] = "";
            qDebug()<<"cam1->stopCamera()";
            cam1->stopCamera();
            cameras[2] = "";
            qDebug()<<"cam2->stopCamera()";
            cam2->stopCamera();
            cameras[3] = "";
            qDebug()<<"cam3->stopCamera()";
            cam3->stopCamera();
            isSound = false;
            isCam1= false;
            isCam2 = false;
            isCam3 = false;
            //procFindConnectedPhones.setArguments({"kill-server"});
        }
        //procFindConnectedPhones.start();
        // firstVideoPacket1.data = QByteArray();
        // firstVideoPacket2.data = QByteArray();
        // firstVideoPacket3.data = QByteArray();
        qDebug()<<"procFindConnectedPhones.start()";
    });
    // procFindConnectedPhones.setArguments({"kill-server"});
    // procFindConnectedPhones.start();
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
    qDebug()<<"slotParams = "<<c<<w<<h<<f<<params;
}

void CameraController::slotCheckDevices(){
    qDebug()<<"procCheckDevices finished";
    QString output = procCheckDevices.readAllStandardOutput();
    qDebug()<<output;
    QStringList list = output.split("\n");
    QStringList dev;
    if(list.at(0).contains("List of devices attached") && list.count() > 1){
        for(int i = 1; i < list.count(); i++){
            if(list.at(i).contains("device")){
                dev.append(list.at(i).split("\t").at(0));
            }
        }
    }
    qDebug()<<"dev = "<<dev<<devices<<cameras;
    if(ui->cbTurnCams->isChecked())
        QTimer::singleShot(1000, this, [this](){procCheckDevices.start();});
    //QTimer::singleShot(1000, &procCheckDevices, SLOT(start()));
}

void CameraController::setStreamUrl(QString s){
    urlStream = s;
    if(streamCam == 1){
        cam1->setStream(urlStream + keyStream);
        cam2->setStream("");
        cam3->setStream("");
    }
    if(streamCam == 2){
        cam1->setStream("");
        cam2->setStream(urlStream + keyStream);
        cam3->setStream("");
    }
    if(streamCam == 3){
        cam1->setStream("");
        cam2->setStream("");
        cam3->setStream(urlStream + keyStream);
    }
}

void CameraController::setStreamKey(QString s){
    keyStream = s;
    if(streamCam == 1){
        cam1->setStream(urlStream + keyStream);
        cam2->setStream("");
        cam3->setStream("");
    }
    if(streamCam == 2){
        cam1->setStream("");
        cam2->setStream(urlStream + keyStream);
        cam3->setStream("");
    }
    if(streamCam == 3){
        cam1->setStream("");
        cam2->setStream("");
        cam3->setStream(urlStream + keyStream);
    }
}

void CameraController::setStreamCam(int cam){
    streamCam = cam;
    if(streamCam == 1){
        cam1->setStream(urlStream + keyStream);
        cam2->setStream("");
        cam3->setStream("");
    }
    if(streamCam == 2){
        cam1->setStream("");
        cam2->setStream(urlStream + keyStream);
        cam3->setStream("");
    }
    if(streamCam == 3){
        cam1->setStream("");
        cam2->setStream("");
        cam3->setStream(urlStream + keyStream);
    }
}

void CameraController::startRecord(bool b, QString s){
    btnPlay->setEnabled(!b);
    btnPlayLast->setEnabled(!b);

    if(threadRecorder->isRunning()) return;
    QString file = "videos/" + s + "_" + QTime::currentTime().toString("hh:mm:ss") + ".mp4";
    file.replace(":", "_");

    if(isCam1 || isCam2 || isCam3){
        // int countCams = 0;
        // if(isCam1) countCams++;
        // if(isCam2) countCams++;
        // if(isCam3) countCams++;

        recorder = new RecordWorker(file, params, {isCam1 == true ? &firstVideoPacket1 : NULL,
                                                   isCam2 == true ? &firstVideoPacket2 : NULL,
                                                   isCam3 == true ? &firstVideoPacket3 : NULL});
        connect(threadRecorder, &QThread::started, recorder, &RecordWorker::start);
        connect(threadRecorder, &QThread::finished, recorder, &RecordWorker::deleteLater);
        connect(this, &CameraController::sigPacket, recorder, &RecordWorker::packetHandler);
        connect(recorder, &RecordWorker::started, this, [this](){
            ui->lblRec->setStatusRec(true);
        });
        connect(recorder, &RecordWorker::stopped, this, [this](){
            ui->lblRec->setStatusRec(false);
        });
        recorder->moveToThread(threadRecorder);
        threadRecorder->start();
    }
}

void CameraController::stopRecord(){
    threadRecorder->quit();
}

void CameraController::setPlayerTv(PlayerTv *p)
{
    player->setTvPlayer(p);
}

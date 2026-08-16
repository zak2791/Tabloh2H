#include "usbcamerawidget.h"
#include "qtcpserver.h"
#include "qtcpsocket.h"
#include <QPainter>

UsbCameraWidget::UsbCameraWidget(QWidget *parent)
    : QOpenGLWidget(parent){

    //qDebug()<<"UsbCameraWidget";
    thread = new QThread(this);
    // qDebug()<<"thread = "<<thread;
    redImage = QImage(320, 240, QImage::Format_RGB32);
    redImage.fill(Qt::red);
    drawFrame(redImage);

    procSetupPorts1.setProgram("platform-tools/adb");
    procSetupPorts2.setProgram("platform-tools/adb");
    procSetupPorts3.setProgram("platform-tools/adb");

    // connect(&procSetupPorts1, &QProcess::started, this, [](){qDebug()<<"procSetupPorts1 started";});
    // connect(&procSetupPorts1, &QProcess::finished, this, [](){qDebug()<<"procSetupPorts1 finished";});

    // connect(&procSetupPorts2, &QProcess::started, this, [](){qDebug()<<"procSetupPorts2 started";});
    // connect(&procSetupPorts2, &QProcess::finished, this, [](){qDebug()<<"procSetupPorts2 finished";});

    // connect(&procSetupPorts3, &QProcess::started, this, [](){qDebug()<<"procSetupPorts3 started";});
    // connect(&procSetupPorts3, &QProcess::finished, this, [](){qDebug()<<"procSetupPorts3 finished";});

    // connect(&procFindDevice, &QProcess::started, this, [](){qDebug()<<"procFindDevice started";});
    // connect(&procFindDevice, &QProcess::finished, this, [](){qDebug()<<"procFindDevice finished";});

    procFindDevice.setProgram("platform-tools/adb");
    procFindDevice.setArguments({"devices"});

    connect(&procFindDevice, &QProcess::readyRead, this, [this](){
        qDebug()<<"procFindDevice";
        QString devices = procFindDevice.readAll();
        if(devices.contains(device)){
            setupPorts();
            // QProcess process;
            // process.setProgram("platform-tools/adb");
            // process.setArguments({"-s", device, "shell", "monkey", "-p", "com.example.replaycamera", "1"});
            // process.start();
            socket.connectToHost(QHostAddress::LocalHost, portConnect);
        }
        else{
            if(isWorking)
                QTimer::singleShot(1000, this, [this](){procFindDevice.start();} );
        }
    });

    connect(&socket, &QAbstractSocket::disconnected, this, [this](){
        drawFrame(redImage);
        if(!isWorking)
            socket.close();
        else{
            procFindDevice.start();
        }

    });

    connect(&socket, &QAbstractSocket::connected, this, [](){
        qDebug()<<"&QAbstractSocket::connected";
        //timer.stop();
    });

    connect(&socket, &QTcpSocket::readyRead, this, [this](){
        //qDebug()<<"QTcpSocket::readyRead "<<socket.readAll();
        socket.readAll();
    });

}

UsbCameraWidget::~UsbCameraWidget() {
    qDebug()<<"~UsbCameraWidget()"<<cameraNumber<<socket.state();
    //worker->deleteLater();
    //worker->stop();
    if(socket.state() == QTcpSocket::ConnectedState)
        socket.write("1");
    socket.close();

    if(thread->isRunning()){
        thread->quit();
        while(!thread->isFinished());
    }

}

void UsbCameraWidget::startCamera(QString dev){
    //////////////////////////////////////////////////////////
    ///
    /// adb -s dev shell monkey -p com.example.replaycamera 1
    /// вывод из фона
    /////////////////////////////////////////////////////////////////////
    qDebug()<<"start camera"<<dev;
    device = dev;
    if(cameraNumber == 1){
        portVideo = 5551;
        portAudio = 5552;
        portConnect = 5553;
    }
    else if(cameraNumber == 2){
        portVideo = 5561;
        portAudio = 5562;
        portConnect = 5563;
    }
    else{
        portVideo = 5571;
        portAudio = 5572;
        portConnect = 5573;
    }

    setupPorts();

    worker = new CameraWorker(cameraNumber,
                              _width,
                              _height,
                              fps,
                              hwDecoder,
                              isSound,
                              urlVk
                              );

    //emit sigParams(cameraNumber, list.at(0).toInt(), list.at(1).toInt(), list.at(2).toInt());
    //thread = new QThread(this);
    connect(worker, &CameraWorker::sigFrame, this, &UsbCameraWidget::drawFrame);
    worker->moveToThread(thread);
    connect(worker, &CameraWorker::sigIsStream, this, &UsbCameraWidget::sigIsStream);
    connect(thread, &QThread::started, worker, &CameraWorker::start);
    connect(thread, &QThread::started, worker, [this](){qDebug()<<"&QThread::started"<<thread;});
    connect(thread, &QThread::finished, worker, &CameraWorker::deleteLater);

    connect(thread, &QThread::finished, this, [this](){
        qDebug()<<"&QThread::finished"<<thread;
        //disconnect(thread);
        //delete thread;
        emit sigCamOff();
    });
    connect(worker, &CameraWorker::sigExit, thread, &QThread::quit);
    connect(worker, &CameraWorker::destroyed, thread, &QThread::quit);
    connect(worker, &CameraWorker::sigVideoPacket, this, &UsbCameraWidget::sigVideoPacket);
    if(isSound || urlVk != "")
        connect(worker, &CameraWorker::sigSoundPacket, this, &UsbCameraWidget::sigSoundPacket);


    thread->start();

    socket.connectToHost(QHostAddress::LocalHost, portConnect);

    isWorking = true;
}

void UsbCameraWidget::stopCamera(){
    qDebug()<<"stop camera"<<socket.state();;
    isWorking = false;
    if(thread->isRunning()){
        //worker->deleteLater();
        qDebug()<<"thread->isRunning()";
        socket.write("1");
        socket.waitForBytesWritten(500);
        socket.disconnectFromHost();
        thread->quit();
        //worker->deleteLater();
        //thread->deleteLater();
    }
    drawFrame(redImage);
    emit sigCamOff();
}

void UsbCameraWidget::setCameraParameters(int cam, int w, int h, int f){
    cameraNumber = cam;
    _width = w;
    _height = h;
    fps = f;
}

void UsbCameraWidget::setupPorts(){

    procSetupPorts1.setArguments({"-s", device, "forward", "tcp:" + QString::number(portConnect), "tcp:" + QString::number(portConnect)});
    procSetupPorts1.start();
    qDebug()<<procSetupPorts1.arguments();
    procSetupPorts2.setArguments({"-s", device, "forward", "tcp:" + QString::number(portVideo), "tcp:" + QString::number(portVideo)});
    procSetupPorts2.start();
    qDebug()<<procSetupPorts2.arguments();
    procSetupPorts3.setArguments({"-s", device, "forward", "tcp:" + QString::number(portAudio), "tcp:" + QString::number(portAudio)});
    procSetupPorts3.start();
    qDebug()<<procSetupPorts3.arguments();
    procSetupPorts1.waitForFinished();
    procSetupPorts2.waitForFinished();
    procSetupPorts3.waitForFinished();
}

void UsbCameraWidget::drawFrame(QImage img){
    image = img;
    update();
}

void UsbCameraWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawImage(QRect(0, 0, width(), height()), image);
    painter.end();
}




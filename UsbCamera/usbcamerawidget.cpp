#include "usbcamerawidget.h"
#include "qtcpserver.h"
#include "qtcpsocket.h"
#include <QPainter>

UsbCameraWidget::UsbCameraWidget(QWidget *parent)
    : QOpenGLWidget(parent){

    qDebug()<<"UsbCameraWidget";
    thread = new QThread(this);
    qDebug()<<"thread = "<<thread;
    redImage = QImage(320, 240, QImage::Format_RGB32);
    redImage.fill(Qt::red);
    drawFrame(redImage);
    connect(&socket, &QAbstractSocket::connected, this, [this](){
        socket.write(QByteArray("parameters"));
    });
    connect(&socket, &QAbstractSocket::readyRead, this, [this](){
        QByteArray ba = socket.readAll();
        QString str = QString::fromUtf8(ba);
        QStringList list = str.split('=');
        if(list.count() == 3){
            worker = new CameraWorker(cameraNumber,
                                      list.at(0).toInt(),
                                      list.at(1).toInt(),
                                      list.at(2).toInt(),
                                      hwDecoder,
                                      isSound,
                                      urlVk
                                      );
            emit sigParams(cameraNumber, list.at(0).toInt(), list.at(1).toInt(), list.at(2).toInt());
            connect(worker, &CameraWorker::sigFrame, this, &UsbCameraWidget::drawFrame);
            worker->moveToThread(thread);
            connect(thread, &QThread::started, worker, &CameraWorker::start);
            //connect(thread, &QThread::destroyed, this, [this](){worker->deleteLater();});
            connect(thread, &QThread::destroyed, this, [this](){qDebug()<<"&QThread::destroyed"<<thread;});
            connect(thread, &QThread::finished, this, [this](){qDebug()<<"&QThread::finished"<<thread;});
            connect(worker, &CameraWorker::sigExit, thread, &QThread::quit);
            connect(worker, &CameraWorker::destroyed, thread, &QThread::quit);
            connect(worker, &CameraWorker::sigVideoPacket, this, &UsbCameraWidget::sigVideoPacket);
            if(isSound || urlVk != "")
                connect(worker, &CameraWorker::sigSoundPacket, this, &UsbCameraWidget::sigSoundPacket);

            //connect(thread, &QThread::finished, thread, &QThread::deleteLater);
            //connect(worker, &CameraWorker::sigExit, this, [this](){drawFrame(redImage);});
            thread->start();
            timer.start(500);
        }
    });

    connect(&socket, &QAbstractSocket::disconnected, this, [this](){
        qDebug()<<"&QAbstractSocket::disconnected";
        drawFrame(redImage);
        timer.stop();
        socket.close();
        // if(thread != NULL){
        //     // qDebug()<<"thread->quit() 0 ";
        //     // thread->quit();
        //     qDebug()<<"thread->quit() 1 ";
        //     thread->deleteLater();
        //     thread = NULL;
        //     worker = NULL;
        // }
        //stopCamera();
    });
    connect(&timer, &QTimer::timeout, this, [this](){
        socket.write(QByteArray("conn_check"));
        socket.flush();
    });
    connect(&tmr, &QTimer::timeout, this, [this](){isUpdate = true;});
    tmr.start(200);



}

UsbCameraWidget::~UsbCameraWidget() {
    qDebug()<<"~UsbCameraWidget()"<<cameraNumber;
    //worker->deleteLater();
    //worker->stop();
    socket.close();

    if(thread->isRunning())
        thread->quit();

}

void UsbCameraWidget::startCamera(){
    int port;
    if(cameraNumber == 1)
        port = 5553;
    else if(cameraNumber == 2)
        port = 5563;
    else
        port = 5573;
    socket.connectToHost(QHostAddress::LocalHost, port);
}

void UsbCameraWidget::stopCamera(){
    if(thread->isRunning()){
        socket.disconnectFromHost();
        worker->deleteLater();
    }
    drawFrame(redImage);
}

void UsbCameraWidget::drawFrame(QImage img){
    image = img;
    qDebug()<<"drawFrame"<<cameraNumber<<img.width()<<img.height();
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




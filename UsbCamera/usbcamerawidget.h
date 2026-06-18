#ifndef USBCAMERAWIDGET_H
#define USBCAMERAWIDGET_H

#include <QOpenGLWidget>
#include "CameraWorker.h"
#include "qtcpserver.h"
#include "qtcpsocket.h"
#include <QTimer>
#include <QThread>


class UsbCameraWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    UsbCameraWidget(QWidget *parent = nullptr);
    ~UsbCameraWidget();
    void startCamera(void);
    void stopCamera(void);
    void setSound(bool b){isSound = b;}
    void setStream(QString s){urlVk = s;}
    void setCameraNumber(int num){cameraNumber = num;}

private:

    QThread* thread = NULL;
    QImage image;
    int cameraNumber;
    QTcpSocket socket;
    QTcpSocket* socketVideo;
    QTimer timer;
    QTimer tmr;
    CameraWorker* worker;
    QTcpServer* serverVideo;
    QString hwDecoder;


    bool isUpdate;
    bool isSound;
    QString urlVk;
    QImage redImage;
    bool isWorking = false;
    int port;


protected:
    void paintEvent(QPaintEvent *event) override;

public slots:
    void drawFrame(QImage);
    void setHwDecoder(QString s){hwDecoder = s;}

private slots:
    void getParameters(void);

signals:
    void sigVideoPacket(packet);
    void sigSoundPacket(packet);
    void sigParams(int, int, int, int);
    void sigCamOff(void);

};
#endif // USBCAMERAWIDGET_H

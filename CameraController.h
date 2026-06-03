#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include "playerpc.h"
#include "qsettings.h"
#include "recordworker.h"
#include "svgbutton.h"
#include "usbcamerawidget.h"
#include <QWidget>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class CameraController : public QWidget
{
    Q_OBJECT

public:
    CameraController(QWidget *parent = nullptr);
    ~CameraController();

    void startRecord(bool, QString);
    void stopRecord(void);

    void setPlayerTv(PlayerTv*);


private:
    Ui::Widget *ui;
    UsbCameraWidget* cam1;
    UsbCameraWidget* cam2;
    UsbCameraWidget* cam3;
    QProcess procFindConnectedPhones;
    QProcess procGetCamParameters;
    QProcess procSetupPorts;
    QStringList devices;
    QMap<int, QString> cameras;
    QMap<int, QList<int>> params;
    int streamCam;
    RecordWorker* recorder;
    QThread* threadRecorder;

    QStringList getCamParameters(QString device);
    SvgButton* btnPlayLast;
    SvgButton* btnPlay;
    QString urlStream;
    QString keyStream;
    bool isSound;
    bool isCam1 = false;
    bool isCam2 = false;
    bool isCam3 = false;
    packet firstVideoPacket1;
    packet firstVideoPacket2;
    packet firstVideoPacket3;

    PlayerPc* player;
    QSettings* settings;

private slots:
    void slotVideo1(packet);
    void slotVideo2(packet);
    void slotVideo3(packet);
    void slotSound(packet);
    void slotParams(int, int, int, int);

public slots:
    void setStreamUrl(QString s){urlStream = s;qDebug()<<urlStream<<keyStream<<streamCam;}
    void setStreamKey(QString s){keyStream = s;qDebug()<<urlStream<<keyStream<<streamCam;}
    void setStreamCam(int cam){streamCam = cam;qDebug()<<urlStream<<keyStream<<streamCam;}

signals:
    void sigShowPlayer(void);
    void sigHidePlayer(void);
    void sigPacket(packet, int);

};
#endif // CAMERACONTROLLER_H

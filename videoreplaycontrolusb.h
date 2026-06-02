#ifndef VIDEOREPLAYCONTROLUSB_H
#define VIDEOREPLAYCONTROLUSB_H

#include "playerpc.h"
#include "playertv.h"
#include "qprocess.h"
#include "qtcpsocket.h"
#include "settingsvideoreplay.h"
#include <QWidget>
#include <QPointer>
#include <QTimer>
#include <QDebug>
#include <ledwidget.h>

namespace Ui {
class VideoreplayControlUsb;
}

class VideoreplayControlUsb : public QWidget
{
    Q_OBJECT

public:
    explicit VideoreplayControlUsb(QWidget *parent = nullptr);
    ~VideoreplayControlUsb();
    void setPlayerTv(PlayerTv*);

private:
    Ui::VideoreplayControlUsb *ui;

    QProcess* procReadCam1;
    QProcess* procReadCam2;
    QProcess* procReadCam3;
    QProcess* procVk;
    QProcess* procProbeAudio;
    QProcess* procAdbFind;
    QProcess* procAdbConnCam;
    QProcess* procAdbConnCams;
    //QStringList argsDevices;

    QMap<int,QString> connectedCameras;
    QTimer* connectionTimer;
    QTcpSocket* connectionSocket;

    QProcess* procRead;
    QProcess* procRecord;
    bool isAudio = false;

    //QProcess* procRecord;

    QString urlCam1;
    QString urlCam2;
    QString urlCam3;
    QString urlWebCam1;
    QString urlWebCam2;
    QString urlSound;

    QString fps1;
    QString fps2;
    QString resolution1;
    QString resolution2;
    QString delaySound;
    QString delayPicture;

    // void turnCam1(void);
    // void turnCam2(void);
    // void turnCam3(void);

    QString urlVk = "";
    QString keyVk = "";
    //bool streamToVk = "";
    int camToVk = 1;

    QTimer* timerCam1;
    QTimer* timerCam2;
    QTimer* timerCam3;

    int countCam1 = 0;
    int countCam2 = 0;
    int countCam3 = 0;
    int countVk = 0;

    QString widthPipVk;
    QString heightPipVk;
    QString transparentPipVk;

    void killFfmpegProcess(void);
    void offStreamVk(void);
    void getHWcodec(void);
    QString hwEncoder = "";
    QString hwDecoder = "";

    void startReadCams(void);
    void stopReadCams(void);

    PlayerPc* player;
    int lastCountFrame;


    QWidget* p;
    SvgButton* btnPlayLast;
    SvgButton* btnPlay;
    QString filterStreamVk;

    SettingsVideoReplay* videoSettings;

    QTimer timerControlFrameDrop;
    int droppedFrames = 0;

    LEDWidget* ledCamera1;
    LEDWidget* ledCamera2;
    LEDWidget* ledCamera3;

    void statesCameras(int state);

    QStringList argsCam1;
    QStringList argsCam2;
    QStringList argsCam3;

private slots:
    void onStreamVk(void);
    void adbFind(void);
    void turnCameras(bool);
    //bool isSound(QString);

public slots:
    void setWebCam1(QString);
    void setWebCam2(QString);
    void setParamWebCam1(QList<int>);
    void setParamWebCam2(QList<int>);
    void setSound(QString);
    void setCam1(QString);
    void setCam2(QString);
    void setCam3(QString);

    void startRecord(bool, QString s = "");
    void stopRecord(bool b = true);
    //void turnStreamToVk(bool b){streamToVk = b;}
    void setUrlVk(QString url){urlVk = url;}
    void setKeyVk(QString key){keyVk = key;}
    void setCamToVk(int cam){camToVk = cam;}
    //void setWidthPipVk(QString width){widthPipVk = width;}
    void setHeightPipVk(QString height){heightPipVk = height;}
    void setTransparentPipVk(QString transparent){transparentPipVk = transparent;}
    void setDelaySound(QString delay){delaySound = delay;}
    void setDelayPicture(QString delay){delayPicture = delay;}
    void showVideoSettings(void){videoSettings->show();}

signals:
    void sigShowPlayer(void);
    void sigHidePlayer(void);
    void sigImage(QImage);
    void sigShowReplayOnTv(bool);
    void sigStateCameras(bool);

};

#endif // VIDEOREPLAYCONTROLUSB_H

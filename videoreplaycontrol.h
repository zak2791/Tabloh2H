#ifndef VIDEOREPLAYCONTROL_H
#define VIDEOREPLAYCONTROL_H

#include "playerpc.h"
#include "qprocess.h"
#include <QWidget>
#include <QPointer>
#include <QTimer>
#include <QDebug>

namespace Ui {
class VideoReplayControl;
}

class VideoReplayControl : public QWidget
{
    Q_OBJECT

public:
    explicit VideoReplayControl(QWidget *parent = nullptr);
    ~VideoReplayControl();
    void setPlayerTv(QMediaPlayer*);

private:
    Ui::VideoReplayControl *ui;

    QProcess* procReadCam1;
    QProcess* procReadCam2;
    QProcess* procReadCam3;
    QProcess* procVk;
    QProcess* procProbeAudio;

    QProcess* procRead;
    QProcess* procRecord;
    bool isAudio = false;

    //QProcess* procRecord;

    QString urlCam1;
    QString urlCam2;
    QString urlCam3;
    QString urlSound;

    QString fps;
    QString resolution;
    QString delaySound;
    QString delayPicture;

    // void turnCam1(void);
    // void turnCam2(void);
    // void turnCam3(void);

    QString urlVk = "";
    QString keyVk = "";
    bool streamToVk = "";
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

    //RecLabel* reclabel;
private slots:
    void onStreamVk(void);

public slots:
    void setWebCam(QString);
    void setParamWebCam(QList<int>);
    void setSound(QString);
    void setCam2(QString);
    void setCam3(QString);

    void startRecord(bool, QString s = "");
    void stopRecord(void);
    void turnStreamToVk(bool b){streamToVk = b;}
    void setUrlVk(QString url){urlVk = url;}
    void setKeyVk(QString key){keyVk = key;}
    void setCamToVk(int cam){camToVk = cam;}
    void setWidthPipVk(QString width){widthPipVk = width;}
    void setHeightPipVk(QString height){heightPipVk = height;}
    void setTransparentPipVk(QString transparent){transparentPipVk = transparent;}
    void setDelaySound(QString delay){delaySound = delay;}
    void setDelayPicture(QString delay){delayPicture = delay;}

signals:
    void sigShowPlayer(void);
    void sigHidePlayer(void);
    void sigImage(QImage);

};

#endif // VIDEOREPLAYCONTROL_H

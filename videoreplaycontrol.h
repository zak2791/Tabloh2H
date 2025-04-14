#ifndef VIDEOREPLAYCONTROL_H
#define VIDEOREPLAYCONTROL_H

#include "playerviewer.h"
#include "qprocess.h"
#include <QWidget>
#include <QPointer>

namespace Ui {
class VideoReplayControl;
}

class VideoReplayControl : public QWidget
{
    Q_OBJECT

public:
    explicit VideoReplayControl(QWidget *parent = nullptr);
    ~VideoReplayControl();

private:
    Ui::VideoReplayControl *ui;

    QProcess procReadCam1;
    QProcess procReadCam2;
    QProcess procReadCam3;

    QProcess procRecord;

    QString urlCam1;
    QString urlCam2;
    QString urlCam3;
    QString urlSound;

    QString fps;
    QString resolution;

    void turnCam1(bool);
    void turnCam2(bool);
    void turnCam3(bool);

    QString urlVk = "";
    QString keyVk = "";
    bool streamToVk = "";
    int camToVk = 1;

    QPointer<PlayerViewer> slowMotionPlayer;

    //RecLabel* reclabel;

public slots:
    void setWebCam(QString);
    void setParamWebCam(QList<int>);
    void setSound(QString);
    void setCam2(QString);
    void setCam3(QString);
    void startRecord(bool, QString s = "");
    void stopRecord();
    void turnStreamToVk(bool b){streamToVk = b;}
    void setUrlVk(QString url){urlVk = url;}
    void setKeyVk(QString key){keyVk = key;}
    void setCamToVk(int cam){camToVk = cam;}

signals:
    void sigShowPlayer(void);
    void sigHidePlayer(void);
    void sigImage(QImage);

};

#endif // VIDEOREPLAYCONTROL_H

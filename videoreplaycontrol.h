#ifndef VIDEOREPLAYCONTROL_H
#define VIDEOREPLAYCONTROL_H

#include "qprocess.h"
#include "reclabel.h"
#include <QWidget>

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

    //RecLabel* reclabel;

public slots:
    void setWebCam(QString);
    void setParamWebCam(QList<int>);
    void setSound(QString);
    void setCam2(QString);
    void setCam3(QString);
    void startRecord(QString s = "");
    void stopRecord();

};

#endif // VIDEOREPLAYCONTROL_H

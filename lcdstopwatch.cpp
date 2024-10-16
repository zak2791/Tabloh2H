#include "lcdstopwatch.h"
#include <QMediaPlayer>
#include <QDebug>
#include <QDir>
#include <QApplication>

LCDStopwatch::LCDStopwatch(QWidget* parent,
                           QString strInitTime,
                           QColor color_stopped,
                           QColor color_works,
                           bool sound,
                           bool transparency
                           ) : LCDTimer(parent, strInitTime, color_stopped, color_works, sound, transparency){

    tmrHide = new QTimer(this);
    connect(tmrHide, &QTimer::timeout, this, &LCDStopwatch::slotTimer);
    Reset();


//#ifdef APP_LAUNCH_FROM_IDE
//    pathToSound = "gong.mp3";
//#else
//    pathToSound = "bin/gong.mp3";
//#endif

}

void LCDStopwatch::StartStop(){
    if(timer->isActive()) {
        timer->stop();
        status = 0;
        setPalette(palStopped);
        emit sigStarted(false);
        tmrHide->start(3000);
    }
    else {
        tmrHide->stop();
        if (status != 2) {
            timer->start(1000);
            status = 1;
            setPalette(palWorks);
            emit sigStarted(true);
        }
    }
    emit sigTime(intTimeToStr(time), palette());
}

void LCDStopwatch::showTime(){
    if(++time == intInitTime){
        status = 2;
        timer->stop();
        setPalette(palStopped);
        emit sigStarted(false);
        if(_sound)
            QApplication::beep();
    }

    QString sTime = intTimeToStr(time);
    display(sTime);
    emit sigTime(sTime, palette());
}

void LCDStopwatch::slotTimer()
{
    tmrHide->stop();
    Reset();
    hide();
}

void LCDStopwatch::Reset(){
    if (status != 1) {
        time = 0;
        status = 0;
        QString sTime = intTimeToStr(0);
        display(sTime);
        emit sigTime(sTime, palette());
        emit sigReset();
    }
}

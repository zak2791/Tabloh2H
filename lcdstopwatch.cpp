#include "lcdstopwatch.h"
#include <QDebug>

LCDStopwatch::LCDStopwatch(QWidget* parent,
                           QString strInitTime,
                           QColor color_stopped,
                           QColor color_works,
                           bool sound,
                           bool transparency
                           ) : LCDTimer(parent, strInitTime, color_stopped, color_works, sound, transparency){

    Reset();

}

void LCDStopwatch::StartStop(){
    if(timer->isActive()) {
        timer->stop();
        status = 0;
        setPalette(palStopped);
        emit sigStarted(false);
    }
    else {
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
        sigStarted(false);
        if(_sound)
            MessageBeep(MB_OK);
    }

    QString sTime = intTimeToStr(time);
    display(sTime);
    emit sigTime(sTime, palette());
    qDebug()<<"status = "<<status;
}

void LCDStopwatch::Reset(){
    if (status != 1) {
        time = 0;
        status = 0;
        QString sTime = intTimeToStr(0);
        display(sTime);
        emit sigTime(sTime, palette());
        sigReset();
    }
}

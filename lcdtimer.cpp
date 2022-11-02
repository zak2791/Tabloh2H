#include "lcdtimer.h"
#include <QDebug>

LCDTimer::LCDTimer(QWidget *parent,
                   QString strInitTime,
                   QColor color_stopped,
                   QColor color_works,
                   bool sound,
                   bool transparency
                   ) : QLCDNumber(parent){

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));

    status = 0;

    _sound = sound;

    intInitTime = strTimeToInt(strInitTime);
    time = intInitTime;

    palStopped = QPalette();
    palStopped.setColor(QPalette::Window, QColor("black"));
    palStopped.setColor(QPalette::WindowText, color_stopped);

    palWorks = QPalette();
    palWorks.setColor(QPalette::Window, QColor("black"));
    palWorks.setColor(QPalette::WindowText, color_works);

    if(transparency)
        setStyleSheet("background-color: rgb(0,0,0,210);");
    setPalette(palStopped);
    setAutoFillBackground(true);

    setDigitCount(4);
    Reset();

}

void LCDTimer::StartStop(){
    if(timer->isActive()) {
        timer->stop();
        status = 0;
        setPalette(palStopped);
        sigStarted(false);
    }
    else {
        if (status != 2) {
            timer->start(1000);
            status = 1;
            setPalette(palWorks);
            sigStarted(true);
        }    
    }
    qDebug()<<objectName()<<time;
    emit sigTime(intTimeToStr(time), palette());
}

void LCDTimer::setTime(int t){
    time = t;
    QString sTime = intTimeToStr(t);
    display(sTime);
    emit sigTime(sTime, palette());
    intInitTime = t;
}

void LCDTimer::Reset(){
    if (status != 1) {
        time = intInitTime;
        status = 0;
        QString sTime = intTimeToStr(intInitTime);
        display(sTime);
        emit sigTime(sTime, palette());
        sigReset();
    }
}

void LCDTimer::showTime(){
    if(--time == 0){
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
}

void LCDTimer::showTime(QString sTime, QPalette pal){
    setPalette(pal);
    display(sTime);
}

int LCDTimer::getStatus(){
    return status;
}

int LCDTimer::strTimeToInt(QString sTime){
    QStringList lTime = sTime.split(":");
    if(lTime.length() != 2)
        return 0;
    bool ok;
    int minutes = lTime[0].toInt(&ok);
    if(!ok)
        return 0;
    int seconds = lTime[1].toInt(&ok);
    if(!ok)
        return 0;
    return minutes * 60 + seconds;
}

QString LCDTimer::intTimeToStr(int iTime){
    int minutes = iTime / 60;
    int secondes = iTime - minutes * 60;
    QString sSeconds;
    if(secondes < 10) sSeconds = "0" + QString::number(secondes);
    else sSeconds = QString::number(secondes);
    return QString::number(minutes) + ":" + sSeconds;
}

void LCDTimer::showEvent(QShowEvent*){
    visible = true;
    sigVisible(true);
}

void LCDTimer::hideEvent(QHideEvent*){
    visible = false;
    sigVisible(false);
}

bool LCDTimer::getVisible(){
    return visible;
}


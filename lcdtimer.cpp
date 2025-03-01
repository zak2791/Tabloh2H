 #include "lcdtimer.h"
#include <QDebug>
#include <QMediaPlayer>
#include <QDir>

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

    if(transparency){
        styleStopped = "QLCDNumber{background-color: rgb(0, 0, 0, 210); "
                                  "color: " + color_stopped.name() + ";}";
        styleWorks = "QLCDNumber{background-color: rgb(0, 0, 0, 210); "
                                "color: " + color_works.name() + ";";
    }
    else{
        styleStopped = "QLCDNumber{background-color: rgb(0, 0, 0); "
                       "border: 5px solid rgb(255, 255, 255); "
                       "border-radius: 20px; "
                       "color: " + color_stopped.name() + ";}";
        styleWorks = "QLCDNumber{background-color: rgb(0, 0, 0); "
                     "border: 5px solid " + color_works.name() + "; "
                     "border-radius: 20px; "
                     "color: " + color_works.name() + ";}";
    }
    //qDebug()<<styleWorks;
    //setStyleSheet("background-color: rgb(0,0,0,210);");





    // styleStopped = styleStopped.arg(bgColor).arg(color_stopped.name());
    // styleWorks = styleWorks.arg(color_works.name());

    setStyleSheet(styleStopped);

    //setContentsMargins(10, 10, 10, 10);

    //qDebug()<<color_stopped.name()<<color_works.name();


    //else
    //setStyleSheet("border-color: rgb(255,0,0,255); border-width: 15px;");

    //setPalette(palStopped);
    setAutoFillBackground(true);

    setDigitCount(4);
    Reset();

#ifdef APP_LAUNCH_FROM_IDE
    pathToSound = "gong.mp3";
#else
    pathToSound = "bin/gong.mp3";
#endif

}

void LCDTimer::StartStop(){
    if(timer->isActive()) {
        timer->stop();
        status = 0;
        //setPalette(palStopped);
        setStyleSheet(styleStopped);
        emit sigStarted(false);
    }
    else {
        if (status != 2) {
            timer->start(1000);
            status = 1;
            //setPalette(palWorks);
            setStyleSheet(styleWorks);
            emit sigStarted(true);
            if(time == intInitTime)
                emit sigStartedInit();
        }    
    }
    emit sigTime(intTimeToStr(time), styleSheet());

}

void LCDTimer::setTime(int t){
    status = 0;
    if(time == intInitTime)
        intInitTime = t;
    time = t;
    QString sTime = intTimeToStr(t);
    display(sTime);
    emit sigTime(sTime, styleSheet());
    //intInitTime = t;
}

void LCDTimer::setTime(int t, int init){
    status = 0;
    //if(time == intInitTime)
    intInitTime = init;
    time = t;
    QString sTime = intTimeToStr(t);
    display(sTime);
    emit sigTime(sTime, styleSheet());
    //intInitTime = t;
}

QString LCDTimer::getTime(){
    return intTimeToStr(time);
}

int LCDTimer::getITime(){
    return time - intInitTime;
}

void LCDTimer::Reset(){
    if (status != 1) {
        time = intInitTime;
        status = 0;
        QString sTime = intTimeToStr(intInitTime);
        display(sTime);
        emit sigTime(sTime, styleSheet());
        emit sigReset();
        emit sigIntTime(time);
    }
}

void LCDTimer::mousePressEvent(QMouseEvent *)
{
    emit sigClicked();
}

void LCDTimer::showTime(){
    if(--time == 0){
        status = 2;
        timer->stop();
        //setPalette(palStopped);
        setStyleSheet(styleStopped);
        emit sigStarted(false);
        if(_sound){
            QMediaPlayer * pPlayer = new QMediaPlayer;
            pPlayer->setMedia(QUrl("qrc:/sound/gong.mp3"));
            emit sigEndTime();
            pPlayer->play() ;
        }
    }

    QString sTime = intTimeToStr(time);
    display(sTime);
    emit sigTime(sTime, styleSheet());
    emit sigIntTime(time);
}

// void LCDTimer::showTime(QString sTime, QPalette pal){
//     setPalette(pal);
//     display(sTime);
// }

void LCDTimer::showTime(QString sTime, QString style){
    //setPalette(pal);
    setStyleSheet(style);
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
    emit sigVisible(true);
}

void LCDTimer::hideEvent(QHideEvent*){
    visible = false;
    emit sigVisible(false);
}

bool LCDTimer::getVisible(){
    return visible;
}


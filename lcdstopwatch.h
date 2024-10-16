#ifndef LCDSTOPWATCH_H
#define LCDSTOPWATCH_H

#include "lcdtimer.h"

class LCDStopwatch : public LCDTimer
{

    QTimer* tmrHide;

public:
    LCDStopwatch(QWidget *parent = 0,
                 QString initTime = "2:00",
                 QColor color_stopped = QColor(0, 150, 0),
                 QColor color_works = QColor(0, 255, 0),
                 bool sound = true,
                 bool transparency = false
                 );

private slots:
    void showTime(void);
    QString pathToSound;
    void slotTimer(void);

public slots:
    void StartStop();
    void Reset();

//signals:
//    void sigEndTime(void);

};

#endif // LCDSTOPWATCH_H

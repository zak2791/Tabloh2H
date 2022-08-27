#ifndef LCDTIMER_H
#define LCDTIMER_H

#include <QLCDNumber>
#include <QWidget>
#include <QPalette>
#include <QTimer>
#include "qt_windows.h"

class LCDTimer : public QLCDNumber
{
    Q_OBJECT
    Q_PROPERTY(bool Visible READ getVisible)
    Q_PROPERTY(int  Status  READ getStatus)

public:
    LCDTimer(QWidget *parent = 0,
             QString initTime = "3:00",
             QColor color_stopped = QColor(0, 150, 0),
             QColor color_works = QColor(0, 255, 0),
             bool sound = true,
             bool transparency = false
             );




    void setTime(int);
    virtual void showEvent(QShowEvent*);
    virtual void hideEvent(QHideEvent*);



private:


    bool visible = false;
    //QString strInitTime;
    int strTimeToInt(QString);
    int getStatus(void);
    bool getVisible(void);

protected:
    int status;     //0 - stopped
                    //1 - works
                    //2 - final state
    QTimer *timer;
    QString intTimeToStr(int);
    QPalette palStopped;
    QPalette palWorks;
    int time;
    int intInitTime;
    bool _sound;


public slots:
    virtual void StartStop();
    void showTime(QString, QPalette);

    virtual void Reset();

    //bool isVisible(void);
    //virtual void setVisible(bool);

protected slots:
    virtual void showTime(void);

signals:
    void sigTime(QString, QPalette);
    void sigStarted(bool);
    void sigReset(void);
    void sigVisible(bool);

};

#endif // LCDTIMER_H

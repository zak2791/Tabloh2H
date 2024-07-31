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
    Q_PROPERTY(int  time    READ getITime)

public:
    LCDTimer(QWidget *parent = 0,
             QString initTime = "3:00",
             QColor color_stopped = QColor(0, 150, 0),
             QColor color_works = QColor(0, 255, 0),
             bool sound = true,
             bool transparency = false
             );



    int getStatus(void);
    int getITime(void);
    void setTime(int);
    void setTime(int, int);
    bool isInitTime(void){return time == intInitTime;}
    QString getTime();
    virtual void showEvent(QShowEvent*);
    virtual void hideEvent(QHideEvent*);


private:


    bool visible = false;
    //QString strInitTime;
    int strTimeToInt(QString);
    bool getVisible(void);
    QString pathToSound;


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
    void sigIntTime(int);
    void sigStarted(bool);
    void sigReset(void);
    void sigVisible(bool);
    void sigEndTime(void);

};

#endif // LCDTIMER_H

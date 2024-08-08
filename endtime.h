#ifndef ENDTIME_H
#define ENDTIME_H

#include <QLabel>
#include <QObject>
#include <QMouseEvent>

class EndTime : public QLabel
{

    Q_OBJECT

    int id_timer;
    int transparent;
    //int finishTransparent;

public:
    EndTime(QWidget *parent = 0);

    virtual void timerEvent(QTimerEvent*);
    //virtual void mousePressEvent(QMouseEvent*);
    void sbros(void);

public slots:
    void startProcess(void);
    //void startTechProcess(void);

};

#endif // ENDTIME_H

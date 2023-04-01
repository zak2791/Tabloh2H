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

public:
    EndTime(QWidget *parent = 0);

    virtual void timerEvent(QTimerEvent*);
    virtual void mousePressEvent(QMouseEvent*);

public slots:
    void startProcess(void);

};

#endif // ENDTIME_H

#ifndef CONTROLOBS_H
#define CONTROLOBS_H

#include <QObject>
#include <QLibrary>

class ControlObs : public QObject
{
    Q_OBJECT
public:
    ControlObs(QString, int, QString);
    ~ControlObs();

private:
    QLibrary* f_Lib;

    int port;
    wchar_t* wIp;
    wchar_t* wPassw;

public slots:
    void doWork(const QString &parameter);

signals:
    void resultReady(const QString &result);
};

#endif // CONTROLOBS_H

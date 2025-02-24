#ifndef CONTROLOBS_H
#define CONTROLOBS_H

#include <QObject>
#include <QLibrary>

class ControlObs : public QObject
{
    Q_OBJECT
public:
    ControlObs();
    ~ControlObs();

private:
    QLibrary* f_Lib;

public slots:
    void doWork(QString, int, QString);

signals:
    void resultReady(const QString &result);
};

#endif // CONTROLOBS_H

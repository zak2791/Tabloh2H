#ifndef CAMERACONNECTION_H
#define CAMERACONNECTION_H

#include <QObject>
#include <QHostAddress>
#include <QUdpSocket>

class CameraConnection : public QObject
{
    Q_OBJECT

    int camera;
    QUdpSocket* udpClient;
    QList<QString> ip;

    virtual void timerEvent(QTimerEvent*);

private slots:
    void clientProcessDatagrams(void);

public:
    explicit CameraConnection(QObject *parent = nullptr, int cam = 1);
    ~CameraConnection();

signals:
    void sigCamera(QString);

};

#endif // CAMERACONNECTION_H

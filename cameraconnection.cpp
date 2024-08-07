#include "cameraconnection.h"
#include <QDebug>
#include <QTimerEvent>

#include <QNetworkInterface>
#include <QDataStream>
#include <QDateTime>
#include <QNetworkDatagram>

CameraConnection::CameraConnection(QObject *parent, int cam) : QObject(parent){
    camera = cam;

    udpClient = new QUdpSocket(this);
    udpClient->bind(20000);
    connect(udpClient, SIGNAL(readyRead()), SLOT(clientProcessDatagrams()));

    startTimer(1000);

    //QList<QString> ip;
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    for(int nIter=0; nIter<list.count(); nIter++){
        if(!list[nIter].isLoopback())
            if (list[nIter].protocol() == QAbstractSocket::IPv4Protocol ){
                ip.append(list[nIter].toString());
            }
    }

    for(int i = 0; i < ip.length(); i++)
        qDebug()<<ip[i];

    qDebug()<<"constructor"<<camera;
}

CameraConnection::~CameraConnection(){
    qDebug()<<"destructor";
}

void CameraConnection::timerEvent(QTimerEvent*){
    QUdpSocket* m_pudp = new QUdpSocket(this);
    QByteArray baDatagram;
    QString s("Camera" + QString::number(camera) + "\0");
    baDatagram.append(s.toUtf8());
    for(int i = 0; i < ip.length(); i++){
        QList<QString> _ip = ip[i].split(".");
        if(_ip.length() == 4){
            m_pudp->writeDatagram(baDatagram, QHostAddress(QString(_ip[0] + "." + _ip[1] + "." + _ip[2] + ".255")), 10000);
        }
    }
    delete m_pudp;
}

void CameraConnection::clientProcessDatagrams(){
    qDebug()<<"datagramm";
    QNetworkDatagram dtg;
    do {
        dtg = udpClient->receiveDatagram();
    }while(udpClient->hasPendingDatagrams());
    QString sIP = dtg.senderAddress().toString();
    if(sIP.contains("::ffff:"))
        sIP = sIP.remove("::ffff:");
    if(dtg.data() == "Camera")
        sigCamera(sIP);
    qDebug()<<dtg.data();
}

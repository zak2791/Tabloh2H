#include "htmlserver.h"

HtmlServer::HtmlServer(QObject *parent)
    : QObject{parent}
{
    server = new QTcpServer(this);
    server->listen(QHostAddress::Any, 50000);
    connect(server,SIGNAL(newConnection()), this, SLOT(slotNewConnection()));

    //connect(server, &QTcpServer::newConnection, this, &HtmlServer::slotNewConnection);

    // if(!server->listen(QHostAddress::Any, 50000)){
    //     qDebug() << "server is not started";
    // } else {
    //     qDebug() << "server is started";
    // }
}

void HtmlServer::slotNewConnection()
{
    qDebug()<<"new conn";
}

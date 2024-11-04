#ifndef HTMLSERVER_H
#define HTMLSERVER_H

#include <QObject>
#include <QTcpServer>

class HtmlServer : public QObject
{
    Q_OBJECT

    QTcpServer* server;

private slots:
    void slotNewConnection(void);

public:
    explicit HtmlServer(QObject *parent = nullptr);

signals:
};

#endif // HTMLSERVER_H

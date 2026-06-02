#include "ledwidget.h"
#include "qpainter.h"

//#include "qtimer.h"

LEDWidget::LEDWidget(int _port, QWidget* parent) : QWidget(parent) {
    status = false;
    port = _port;
    setMinimumSize(10, 10);

    timer = new QTimer(this);
    socket = new QTcpSocket(this);
    connect(timer, &QTimer::timeout, this, [this](){
        if(socket->state() == QAbstractSocket::UnconnectedState){
            socket->connectToHost("127.0.0.1", port);
            updateStatus(false);
        }else{
            socket->write("hello");
        }
    });

    connect(socket, &QTcpSocket::readyRead, this, [this](){
        QByteArray ba = socket->readAll();
        qDebug()<<"connected camera "<<ba<<port;
        if(ba.size() > 0){
            int h = ba.at(0);
            qDebug()<<"connected camera "<<h<<port;
            updateStatus(true);
        }else{
            updateStatus(false);
        }
    });

    connect(socket, &QTcpSocket::errorOccurred, this, [this](){
        qDebug()<<"errorOccurred "<<port<<socket->errorString()<<socket->error();
        if(socket->error() != QAbstractSocket::RemoteHostClosedError)
            updateStatus(false);
    });
    timer->start(1000);
}

void LEDWidget::stop()
{
    timer->stop();
}

void LEDWidget::start()
{
    timer->start(1000);
}

void LEDWidget::updateStatus(bool _status)
{
    qDebug()<<_status<<port;
    status = _status;
    update();
}

void LEDWidget::paintEvent(QPaintEvent*)  {
    QPainter painter(this);
    // Рисуем светодиод
    if (!status) {
        painter.setBrush(QBrush(Qt::red));
    }
    else {
        painter.setBrush(QBrush(Qt::green));
    }


    // Рисуем круг (светодиод)
    painter.drawEllipse(width() * 0.1, height() * 0.1, width() * 0.8, height() * 0.8);
}

void LEDWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    int newSize = std::min(width(), height());
    resize(newSize, newSize);
}


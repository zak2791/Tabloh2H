#ifndef LEDWIDGET_H
#define LEDWIDGET_H

#include "qtcpsocket.h"
#include "qwidget.h"
#include "qtimer.h"

class LEDWidget : public QWidget {
    Q_OBJECT

public:
    LEDWidget(int, QWidget* parent = nullptr);
    void start(void);
    void stop(void);
    void setPort(int);

protected:
    void paintEvent(QPaintEvent* event) override;

    void resizeEvent(QResizeEvent* event) override;

// private slots:
//     void setStatus(int);

private:
    bool status;    // Состояние светодиода


    QTcpSocket* socket;
    QTimer* timer;
    int port;
    void updateStatus(bool);

};

#endif // LEDWIDGET_H

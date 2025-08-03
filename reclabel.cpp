#include "reclabel.h"
#include "qpainter.h"
#include "qtimer.h"
#include <QResizeEvent>

RecLabel::RecLabel(QWidget *parent) : QLabel{parent}
{
    rendererRecNoActive = new QSvgRenderer(path_no_active);
    rendererRecNoActive->setAspectRatioMode(Qt::KeepAspectRatio);
    rendererRecBlink    = new QSvgRenderer(path_blink);
    rendererRecBlink->setAspectRatioMode(Qt::KeepAspectRatio);
    rendererRec         = new QSvgRenderer(path);
    rendererRec->setAspectRatioMode(Qt::KeepAspectRatio);
    rendererRecCurrent = rendererRecNoActive;
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this](){
        if(rendererRecCurrent == rendererRecNoActive)
            return;
        if(rendererRecCurrent == rendererRec)
            rendererRecCurrent = rendererRecBlink;
        else
            rendererRecCurrent = rendererRec;
        repaint();
    });
    timer->start(500);
}

void RecLabel::setStatusRec(bool b)
{
    if(b)
        rendererRecCurrent = rendererRec;
    else
        rendererRecCurrent = rendererRecNoActive;
    repaint();
}

void RecLabel::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev)
    QPainter painter(this);
    QRect rect(0, 0, width(), height());
    rendererRecCurrent->render(&painter, rect);
}

void RecLabel::resizeEvent(QResizeEvent *ev)
{
    int h = ev->size().height();
    setMinimumWidth(h * 2);
}

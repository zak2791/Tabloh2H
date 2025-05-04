#include "vklabel.h"
#include "qpainter.h"
#include "qtimer.h"
#include <QResizeEvent>

VkLabel::VkLabel(QWidget *parent) : QLabel{parent}
{
    rendererVkNoActive = new QSvgRenderer(path_no_active);
    rendererVkBlink    = new QSvgRenderer(path_blink);
    rendererVk         = new QSvgRenderer(path);
    rendererVkCurrent = rendererVkNoActive;
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this](){
        if(rendererVkCurrent == rendererVkNoActive)
            return;
        if(rendererVkCurrent == rendererVk)
            rendererVkCurrent = rendererVkBlink;
        else
            rendererVkCurrent = rendererVk;
        repaint();
    });
    timer->start(500);
}

void VkLabel::setStatusVk(bool b)
{
    if(b)
        rendererVkCurrent = rendererVk;
    else
        rendererVkCurrent = rendererVkNoActive;
    repaint();
}

void VkLabel::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev)
    QPainter painter(this);
    QRect rect(0, 0, width(), height());
    rendererVkCurrent->render(&painter, rect);
}

void VkLabel::resizeEvent(QResizeEvent *ev)
{
    int h = ev->size().height();
    setMaximumWidth(h);
}

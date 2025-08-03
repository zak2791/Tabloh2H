#include "lostlabel.h"
#include "qevent.h"
#include "qpainter.h"

LostLabel::LostLabel(QWidget *parent) : QLabel{parent}
{

    rendererLostActive = new QSvgRenderer(path_active);
    rendererLostActive->setAspectRatioMode(Qt::KeepAspectRatio);
    rendererLostNoActive = new QSvgRenderer(path_no_active);
    rendererLostNoActive->setAspectRatioMode(Qt::KeepAspectRatio);

    lostFrames = 0;

}

void LostLabel::setLostFrames(int frames)
{
    lostFrames = frames;
    repaint();
}

void LostLabel::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev)
    QPainter painter(this);
    QRect rect(0, 0, width(), height());
    if(lostFrames > 0){
        rendererLostActive->render(&painter, rect);
        QFont font = painter.font();
        font.setPixelSize(height() / 2);
        painter.setPen(QPen(Qt::red));
        painter.setFont(font);
        painter.drawText(rect, Qt::AlignCenter, QString::number(lostFrames));
    }
    else{
        rendererLostNoActive->render(&painter, rect);
    }
}

void LostLabel::resizeEvent(QResizeEvent *ev)
{
    int h = ev->size().height();
    setMinimumWidth(h);

}

#include "logofrb.h"

#include <QPainter>

logofrb::logofrb(QWidget *parent) : QWidget(parent)
{

}

void logofrb::paintEvent(QPaintEvent*){
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/images/frb.png");
    int ratio = pix.width() / pix.height();
    QRect rect((width() - height() * ratio) / 2, 0, height() * ratio, height());
    painter.setOpacity(0.8);
    painter.drawPixmap(rect, pix);
}

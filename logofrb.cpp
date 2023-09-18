#include "logofrb.h"

#include <QPainter>

logofrb::logofrb(QWidget *parent) : QWidget(parent){
    // setObjectName("logo");
}

void logofrb::paintEvent(QPaintEvent*){
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/images/frb.png");
    int ratio = pix.width() / pix.height();
    QRect rect((width() - height() * ratio) / 2, 0, height() * ratio, height());
    painter.drawPixmap(rect, pix);
}

void logofrb::on_logo(){
    setVisible(true);
}

void logofrb::off_logo(){
    setVisible(false);
}

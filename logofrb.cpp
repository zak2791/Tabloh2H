#include "logofrb.h"

#include <QPainter>
#include <QDebug>

logofrb::logofrb(QWidget *parent) : QWidget(parent){
#ifdef APP_LAUNCH_FROM_IDE
    settings = new QSettings("settings.ini", QSettings::IniFormat);
#else
    settings = new QSettings("bin/settings.ini", QSettings::IniFormat);
#endif
    settings->beginGroup("logo");
    if(settings->value("logo", 1).toInt())
        pathLogo = ":/images/frb.png";
    else
        pathLogo = ":/images/ihca.png";
    settings->endGroup();
}

void logofrb::paintEvent(QPaintEvent*){
    QPainter painter(this);
    QPixmap pix;
    pix.load(pathLogo);
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

void logofrb::setLogo(bool logo)
{
    settings->beginGroup("logo");
    if(logo){
        pathLogo = ":/images/frb.png";
        settings->setValue("logo", 1);
    }
    else{
        pathLogo = ":/images/ihca.png";
        settings->setValue("logo", 0);
    }
    settings->endGroup();
    repaint();
}

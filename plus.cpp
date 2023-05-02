#include "plus.h"
#include <QDebug>
#include <QPainter>

Plus::Plus(QString col, QWidget *parent) : QLabel(parent)
{
    setColor("black");
	/*
	QPalette palW = palette();
	palW.setColor(QPalette::WindowText, QColor(col));
	setPalette(palW);
	setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	*/
    pix_plus = new QPixmap(":/images/plus.png");
    //setAutoFillBackground(true);
};

Plus::~Plus()
{
}

void Plus::resizeEvent(QResizeEvent * ) {
    //QFont fnt;
    //fnt.setWeight(50);
    //fnt.setPixelSize(height() * 0.8);
    //setFont(fnt);
}

void Plus::setData(QString s){
    Text = s;
    if(s == "+"){
        QPixmap pix = pix_plus->scaled(width(), height(), Qt::KeepAspectRatio);
        setPixmap(pix);

    }else{
        QPixmap pix = QPixmap(width(), height());
        pix.fill(Qt::transparent);
        setPixmap(pix);
    }
    emit textChange(Text);
}

QString Plus::getText() {
    return Text;
}

void Plus::setColor(QString col) {
	QPalette palW = palette();
	palW.setColor(QPalette::WindowText, QColor(col));
	setPalette(palW);
    //setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}


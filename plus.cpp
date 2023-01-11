#include "plus.h"
#include <QDebug>

Plus::Plus(QString col, QWidget *parent) : QLabel(parent)
{
	setColor(col);
	/*
	QPalette palW = palette();
	palW.setColor(QPalette::WindowText, QColor(col));
	setPalette(palW);
	setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	*/
};

Plus::~Plus()
{
}

void Plus::resizeEvent(QResizeEvent * ) {
	QFont fnt;
	fnt.setWeight(50);
    fnt.setPixelSize(height() * 0.8);
	setFont(fnt);
}

void Plus::setData(QString s){
    setText(s);
    emit textChange(text());
}

QString Plus::getText() {
	return text();
}

void Plus::setColor(QString col) {
	QPalette palW = palette();
	palW.setColor(QPalette::WindowText, QColor(col));
	setPalette(palW);
	setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}

#include "narusheniya_vyhod.h"
#include <QtCore>
#include <QMouseEvent>



NV::NV(QString col, QLabel *parent) : QLabel(parent)
{
	bl = "";
	id = 0;

	QFile styleFile;

	styleFile.setFileName("style.qss");
	styleFile.open(QFile::ReadOnly);
	style = styleFile.readAll();
	setStyleSheet(style);
	styleFile.close();

	styleFile.setFileName("style_yellow.qss");
	styleFile.open(QFile::ReadOnly);
	style_yellow = styleFile.readAll();
	styleFile.close();

	styleFile.setFileName("style_green.qss");
	styleFile.open(QFile::ReadOnly);
	style_green = styleFile.readAll();
	styleFile.close();

	QPalette palW = palette();
	palW.setColor(QPalette::WindowText, QColor(col));
	setPalette(palW);
	setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

};



NV::~NV()
{
}

void NV::sbros() {
	bl = u8"";
	setText(bl);
	setStyleSheet(style);
	emit ball(bl);
}

void NV::plus() {
	if (QString::compare(bl, u8"") == 0) {
        bl = u8"ЗВ";
		setText(bl);
		setStyleSheet(style_green);
	}
    else if (QString::compare(bl, u8"ЗВ") == 0) {
        bl = u8"В";
		setText(bl);
		setStyleSheet(style_yellow);
	}
	emit ball(bl);
}

void NV::minus() {
    if (QString::compare(bl, u8"В") == 0) {
        bl = u8"ЗВ";
		setText(bl);
		setStyleSheet(style_green);
	}
    else if (QString::compare(bl, u8"ЗВ") == 0) {
		bl = u8"";
		setText(bl);
		setStyleSheet(style);
	}
	emit ball(bl);
}

void NV::setValue(QString b) {
	bl = b;
    if (QString::compare(bl, u8"В") == 0) {
		setText(bl);
		setStyleSheet(style_yellow);
	}
    else if (QString::compare(bl, u8"ЗВ") == 0) {
		setText(bl);
		setStyleSheet(style_green);
	}
	else if (QString::compare(bl, u8"") == 0) {
		setText(bl);
		setStyleSheet(style);
	}
}

void NV::mousePressEvent(QMouseEvent *  pe) {
	if (pe->buttons() == Qt::LeftButton)
		plus();
	else if (pe->buttons() == Qt::RightButton)
		minus();
	//emit change_nv(bl);
}

void NV::resizeEvent(QResizeEvent * ) {
	QFont fnt;
	fnt.setWeight(50);
	int a = height() * 0.75;
	fnt.setPixelSize(a);
	setFont(fnt);
}

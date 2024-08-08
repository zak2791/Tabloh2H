#include "category.h"
#include <QDebug>

Category::Category(QString col, QWidget *parent) : QLabel(parent)
{
	QPalette palW = palette();
	palW.setColor(QPalette::WindowText, QColor(col));
	//palW.setColor(QPalette::Background, QColor("black"));
	setPalette(palW);
	setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
};

Category::~Category()
{
}

void Category::resizeEvent(QResizeEvent * ) {
	QFont fnt;
    fnt.setWeight(QFont::Weight::DemiBold);
	fnt.setPixelSize(height() * 0.8);
	setFont(fnt);
}

void Category::setData(QString s) {
	setText(s);
    emit sigText(s);
}

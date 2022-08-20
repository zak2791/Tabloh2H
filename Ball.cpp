#include <QtCore>
#include <QMouseEvent>
#include "Ball.h"
#include <QMessageBox>
//#include <QDebug>

Ball::Ball(QWidget *parent,
		   int _max_num,
		   bool _diff_font):

QLabel(parent)
{
	bl = 0;
	id = 0;
	max_num = _max_num;
	if (max_num < 10)
		diff_font = false;
	else
		diff_font = _diff_font;

	setStyleSheet("QLabel{border-radius: 30px; border-width: 2px; border-style: solid; border-color:white; background-color: black; color: white;}");

	setAutoFillBackground(true);
	setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

	fnt1 = QFont();
	fnt2 = QFont();
	show();
	
};



Ball::~Ball()
{
}

void Ball::plus() {
	if (bl < max_num) {
		bl++;
		if (bl == 10 && diff_font)
			setFont(fnt2);
		setText(QString::number(bl));
		emit ball(bl);
	}
}

void Ball::minus() {
	if (bl > 0) {
		bl--;
		if (bl == 9 && diff_font)
			setFont(fnt1);
		setText(QString::number(bl));
		emit ball(bl);
	}
}

void Ball::sbros() {
	bl = 0;
	setBall(bl);
    //emit ball(bl);
}

void Ball::mousePressEvent(QMouseEvent *  pe) {
	if (pe->buttons() == Qt::LeftButton)
		plus();
	else if (pe->buttons() == Qt::RightButton)
		minus();
}

void Ball::setBall(int b) {
	if (diff_font == true) {
		if (b < 10)
			setFont(fnt1);
		else
			setFont(fnt2);
	}
	bl = b;
	setText(QString::number(bl));
    emit ball(bl);
}

void Ball::setViewStyle(int s, int width){
	if(s == 0)
		setStyleSheet("QLabel{border-radius: 30px; border-width: " + QString::number(width) + "px; border-style: solid; border-color:white; background-color: black; color: white;}");
	else if(s == 1)
		setStyleSheet("QLabel{border-radius: 30px; border-width: " + QString::number(width) + "px; border-style: solid; border-color:white; background-color: black; color: red;}");
	else if (s == 2)
		setStyleSheet("QLabel{border-radius: 30px; border-width: " + QString::number(width) + "px; border-style: solid; border-color:white; background-color: black; color: blue;}");
	else if (s == 3)
		setStyleSheet("QLabel{background-color: black; color: red; border-width: 0px; border-style: solid;}");
	else if (s == 4)
		setStyleSheet("QLabel{background-color: black; color: blue; border-width: 0px; border-style: solid;}");
}

void Ball::resizeEvent(QResizeEvent * ){
	fnt1.setWeight(50);						//шрифт большой при баллах < 10 и разном размере шрифтов или при макс. сумме < 10
	fnt1.setPixelSize(height() * 2);
	QFontMetrics fm = QFontMetrics(fnt1);
	fnt2.setWeight(50);						//шрифт маленький при баллах > 10 или при < 10 если одинаковый размер шрифтов
	fnt2.setPixelSize(height() * 2);
	int a = height();
	QString st1 = "0";
	QString st2 = "00";
	a = height() * 1.05;
	if (max_num < 10 || diff_font) {
		while (true) {
			fnt1.setPixelSize(a);
			setFont(fnt1);
			fm = QFontMetrics(fnt1);
			if (fm.width(st1) < width() )
				break;
			if (a > 20)
				a -= 20;
			else
				break;
		}
	}
	a = height() * 1.05;
	if (max_num > 9) {
		while (true) {
			fnt2.setPixelSize(a);
			setFont(fnt2);
			fm = QFontMetrics(fnt2);
			if (fm.width(st2) < width())
				break;
			if (a > 20)
				a -= 20;
			else
				break;
		}
	}
	if(diff_font == false && max_num > 9)
		setFont(fnt2);
	else
		setFont(fnt1);
	setText(QString::number(bl));
}

#include "narusheniya_pravil.h"
#include <QtCore>
#include <QMouseEvent>
#include "qt_windows.h"
#include <QTime>
#include <QDebug>

NP::NP(QLabel *parent) : QLabel(parent)
{
	bl = "";

	QFile styleFile;

	style =			QString("QLabel{background-color: black; border-radius: 30px; border-color:white; border-width: 2px; border-style: solid; color: white}");
	style_red =		QString("QLabel{background-color: red; border-radius: 30px; border-color:white; border-width: 2px; border-style: solid; color: black}");
	style_yellow =	QString("QLabel{background-color: yellow; border-radius: 30px; border-color:white; border-width: 2px; border-style: solid; color: black}");
	style_green =	QString("QLabel{background-color: green; border-radius: 30px; border-color:white; border-width: 2px; border-style: solid; color: black }");

	setStyleSheet(style);
	setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

	QThread * thread = new QThread;
	Blinks * blinks = new Blinks();
	blinks->moveToThread(thread);
	connect(blinks, SIGNAL(blink_turn(bool)), this, SLOT(prg(bool)));

	connect(thread, SIGNAL(started()), blinks, SLOT(process()));

	thread->start();
};



NP::~NP()
{
}

void NP::prg(bool b) {
	if (QString::compare(bl, u8"П2") == 0) {
		if (b)
			setStyleSheet(style);
		else
			setStyleSheet(style_red);
	}
}

void NP::sbros() {
	bl = u8"";
	setText(bl);
	setStyleSheet(style);
	emit ball(bl);
}

void NP::set_red(){
	if (QString::compare(bl, u8"П2") == 0)
		setStyleSheet(style_red);
}

void NP::set_black(){
	if (QString::compare(bl, u8"П2") == 0)
		setStyleSheet(style);
}

void NP::plus(){
	if (QString::compare(bl, u8"") == 0) {
		bl = u8"ЗП";
		setText(bl);
		setStyleSheet(style_green);
	}
	else if (QString::compare(bl, u8"ЗП") == 0) {
		bl = u8"П1";
		setText(bl);
		setStyleSheet(style_yellow);
	}
	else if (QString::compare(bl, u8"П1") == 0) {
		bl = u8"П2";
		setText(bl);
		setStyleSheet(style_red);
	}
	emit ball(bl);
}

void NP::minus(){
	if (QString::compare(bl, u8"П2") == 0) {
		bl = u8"П1";
		setText(bl);
		setStyleSheet(style_yellow);
	}
	else if (QString::compare(bl, u8"П1") == 0) {
		bl = u8"ЗП";
		setText(bl);
		setStyleSheet(style_green);
	}
	else if (QString::compare(bl, u8"ЗП") == 0) {
		bl = u8"";
		setText(bl);
		setStyleSheet(style);
	}
	emit ball(bl);
}

void NP::setValue(QString b){
	bl = b;
	if (QString::compare(bl, u8"П2") == 0) {
		setText(bl);
		setStyleSheet(style_red);
	}
	else if (QString::compare(bl, u8"П1") == 0){
		setText(bl);
		setStyleSheet(style_yellow);
	}
	else if (QString::compare(bl, u8"ЗП") == 0) {
		setText(bl);
		setStyleSheet(style_green);
	}
	else if (QString::compare(bl, u8"") == 0) {
		setText(bl);
		setStyleSheet(style);
	}
}

void NP::mousePressEvent(QMouseEvent *  pe) {
	if (pe->buttons() == Qt::LeftButton)
		plus();
	else if (pe->buttons() == Qt::RightButton)
		minus();
	//emit change_np(bl);
}

void NP::resizeEvent(QResizeEvent * ) {
	QFont fnt;
	fnt.setWeight(50);
	QFontMetrics fm(fnt);
	int a = height() * 0.75;
	fnt.setPixelSize(a);
	setFont(fnt);
}

Blinks::Blinks() {
	current_state = false;
}

void Blinks::process() {
	int time_ms;
	while (true) {
		time_ms = QTime::currentTime().msec();
		if (time_ms > 0 && time_ms < 19 && !current_state) {
			emit blink_turn(true);
			current_state = true;
		}
		else if (time_ms > 500 && time_ms < 519 && current_state) {
			emit blink_turn(false);
			current_state = false;
		}
		Sleep(10);
	}
}

Blinks::~Blinks(){
}

#include <QtCore>
#include "Secundomer.h"
#include "qt_windows.h"
#include <QDebug>

Secundomer::Secundomer(QWidget *parent,
					   QEvent * _evl,
					   QEvent * _evr,
					   QString _col_start,
					   QString _col_stop,
					   double __tm,
					   int _drc,
					   bool _bg,
					   bool _beep) :
	QLCDNumber(parent)
{
	drc = _drc;
	setSegmentStyle(Filled);
	setDigitCount(4);
	//QFile styleFile(":/style_green.qss");
	//styleFile.open(QFile::ReadOnly);
	if (_bg)
		setStyleSheet("background-color: rgb(0,0,0,210);");
	//else
		//setStyleSheet(styleFile.readAll());
	//styleFile.close();
	fl_val = 1;
	fl_state = false;
	beep = _beep;

	setFrameShape(QFrame::Box);
	timer = new QTimer();
	connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
	_tm = __tm;

	pal_start = QPalette();
	pal_start.setColor(QPalette::Window, QColor("black"));
	pal_start.setColor(QPalette::WindowText, QColor(_col_start));

	pal_stop = QPalette();
	pal_stop.setColor(QPalette::Window, QColor("black"));
	pal_stop.setColor(QPalette::WindowText, QColor(_col_stop));

	setPalette(pal_stop);
	setAutoFillBackground(true);

	clearTimer();

	setSegmentStyle(QLCDNumber::Filled);
	setFrameShape(QFrame::Box);
	setLineWidth(2);

	evl = _evl;
	evr = _evr;
}

Secundomer::~Secundomer()
{
}

void Secundomer::startstop(){
	if (timer->isActive()) {
		timer->stop();
		fl_state = false;
		setPalette(pal_stop);
        //emit(SIGNAL("state"), 0);
		if (beep)
			MessageBeep(MB_OK);
	}
	else {
		if (fl_val != -1) {
			timer->start(1000);
			fl_state = true;
			setPalette(pal_start);
            //emit(SIGNAL("state"), 1);
            //if (fl_val == 1)
            startRec();
            qDebug()<<"secStart";
			fl_val = 0;
			if (beep)
				MessageBeep(MB_OK);
		}
	}
    sec_sig(text, palette());
    sec_visible(isVisible());
}

void Secundomer::showTime() {
	if (!(timer->isActive() == false && drc == 1 && time->hour() == 0
												 && time->minute() == 0
												 && time->second() == 0
												 && time->msec() == 0)) {
		QTime time2 = time->addSecs(drc);
		time->setHMS(time2.hour(), time2.minute(), time2.second(), time2.msec());
	}
	text = time->toString("mm:ss");
	display(text);
	//emit(SIGNAL("sek"), text);

	if ((drc == -1 && text == "00:00") || (drc == 1 && QString::compare(text, "0" + QString::number(_tm)[0] + ":" + QString::number(_tm)[2] + "0") == 0)) {
		timer->stop();
		MessageBeep(MB_OK);
		setPalette(pal_stop);
        //emit(SIGNAL("state"), 0);
		fl_state = false;
		fl_val = -1;
        //sec_sig(text, palette());
		//sound();
	}
    sec_sig(text, palette());
}

void Secundomer::doubleTime(QString t, QPalette p) {
	display(t);
	setPalette(p);
}

void Secundomer::clearTimer(){
	if (!timer->isActive()) {
		if (drc == 1)
			time = new QTime(0, 0, 0, 0); //time->setHMS(0, 0, 0, 0);
		else
			time = new QTime(0, static_cast<int>(_tm), round((_tm - static_cast<int>(_tm)) * 100 - drc), 0); 
		showTime();
		fl_val = 1;
        stopRec();
        qDebug()<<"secClear";
		fl_state = false;
	}
}

bool Secundomer::getVisible(){
	return isVisible();
}

void Secundomer::customEvent(QEvent * pe){
	if (pe->type() == evl->type()) {
		if (fl_val != -1)
			startstop();
	}
	else {
		if (pe->type() == evr->type())
			clearTimer();
	}
}

void Secundomer::setTime(int m, int s1, int s2){
    _tm = m + s1 / 10.0 + s2 / 100.0;
    clearTimer();
}

bool Secundomer::isActive(){
    return timer->isActive();
}

void Secundomer::showLastTime(QString s){
    time->setHMS(0, s.split(":")[0].toInt(), s.split(":")[1].toInt());
    QString t = time->toString("mm:ss");
    display(t);
    sec_sig(t, palette());
}

QString Secundomer::getLastTime(){
    return time->toString("m:ss");
}


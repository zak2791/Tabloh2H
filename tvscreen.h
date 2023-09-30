#pragma once
#include <QWidget>
#include <QPushButton>
#include "lcdtimer.h"
#include <QGridLayout>
#include <QLabel>
#include "narusheniya_pravil.h"
#include "rate.h"
#include "ui_FormView.h"
#include "fam_reg.h"
#include "plus.h"
#include "category.h"
#include "lcdstopwatch.h"
#include "firstlastname.h"
#include "endtime.h"
#include "logofrb.h"

#include <QSvgRenderer>
#include <QPainter>

class SvgLabel : public QLabel{

public:
        SvgLabel(const QString& iconPath){
            path = iconPath;
        }

    void paintEvent(QPaintEvent* ev){
        QLabel::paintEvent(ev);
        QPainter painter(this);
        QSvgRenderer renderer(path);
        renderer.render(&painter);
    }

private:
    QString path;

};

class TVScreen : public QWidget {
	Q_OBJECT
public:
    explicit TVScreen(QWidget *parent = 0);

    ~TVScreen();

    Rate * ball_red;
    Rate * ball_blue;
    //Rate * akt_red;
    //Rate * akt_blue;

    EndTime * lblEndTimer;         //красный фон по окончании времени

	NP * np_red;
	NP * np_blue;

    FirstLastName * fam_red;
    FirstLastName * fam_blue;
    Fam * fam_next_red;
    Fam * fam_next_blue;
	Fam * reg_red;
	Fam * reg_blue;
	Plus * plus_red;
	Plus * plus_blue;
    LCDTimer * sec;
    LCDStopwatch * sec_red;
    LCDStopwatch * sec_blue;
    LCDStopwatch * sec_red_t;
    LCDStopwatch * sec_blue_t;
    LCDStopwatch * sec_doctor;

    //Category * cat;
    QPushButton* cat;
    QPushButton* age;

	QGridLayout * grid;

	int HEIGHT_REGION;
	int HEIGHT_FAMILY;
	int minimum_height;
	int percent_height;
	QLabel * flag_blue;
	QLabel * flag_red;

    logofrb* logo;

	int View;									//вид табло: 0-красно-синий фон, 
												//			 1-чёрный фон с рамками,
												//			 2-чёрный фон без рамок

private:
	
	virtual void paintEvent(QPaintEvent *);
	virtual void showEvent(QShowEvent *);
    virtual void resizeEvent(QResizeEvent *);
	QString col_red;
	QString col_blue;
	QEvent * ev_L;
	QEvent * ev_R;
	int minimum_height_family;
	int minimum_height_region;



};

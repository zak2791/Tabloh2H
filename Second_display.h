#pragma once
#include <QWidget>
#include <QPushButton>
#include "Secundomer.h"
#include <QGridLayout>
#include <QLabel>
#include "narusheniya_pravil.h"
#include "Ball.h"
#include "narusheniya_vyhod.h"
#include "ui_FormView.h"
#include "fam_reg.h"
#include "plus.h"
#include "replayviewer.h"
#include "category.h"

class SecondDisplay : public QWidget {
	Q_OBJECT
public:
	explicit SecondDisplay(QWidget *parent = 0);

	~SecondDisplay();

	Ball * ball_red;
	Ball * ball_blue;
	Ball * akt_red;
	Ball * akt_blue;

	NP * np_red;
	NP * np_blue;
	NV * nv_red;
	NV * nv_blue;

	Fam * fam_red;
	Fam * fam_blue;
	Fam * reg_red;
	Fam * reg_blue;
	Plus * plus_red;
	Plus * plus_blue;
	Secundomer * sec;
	Secundomer * sec_red;
	Secundomer * sec_blue;
    Secundomer * sec_red_t;
    Secundomer * sec_blue_t;

    Category * cat;

	QGridLayout * grid;

	int HEIGHT_REGION;
	int HEIGHT_FAMILY;
	int minimum_height;
	int percent_height;
	QLabel * flag_blue;
	QLabel * flag_red;
	int View;									//вид табло: 0-красно-синий фон, 
												//			 1-чёрный фон с рамками,
												//			 2-чёрный фон без рамок
    ReplayViewer* rV;

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

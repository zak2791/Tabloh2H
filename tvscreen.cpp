
#include <QPainter>
#include <QGridLayout>
#include <QPushButton>
#include <QEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QDebug>
#include "filter.h"
#include <QMessageBox>
#include <QPalette>

#include "tvscreen.h"
#include "list_family.h"
#include "category.h"
#include "lcdstopwatch.h"
#include <QScreen>

TVScreen::TVScreen(QWidget * parent) : QWidget(parent) {

    QLabel* lblNextRed = new QLabel("");
    lblNextRed->setPixmap(QPixmap(":/images/next_red.png"));
    lblNextRed->setScaledContents( true );
    lblNextRed->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

    QLabel* lblNextBlue = new QLabel("");
    lblNextBlue->setPixmap(QPixmap(":/images/next_blue.png"));
    lblNextBlue->setScaledContents( true );
    lblNextBlue->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

	col_red = "white";
	col_blue = "white";

	View = 0;			//красно-синий фон

    ball_red = new Rate(this);
	ball_red->setFrameShape(QFrame::Box);
	//ball_red->id = 1;
	//ball_red->setObjectName("ball_red");

    ball_blue = new Rate(this);
	ball_blue->setFrameShape(QFrame::Box);
	//ball_blue->id = 0;
	//ball_blue->setObjectName("ball_blue");

    akt_red = new Rate(this);
	akt_red->setFrameShape(QFrame::Box);
	//akt_red->id = 3;
	//akt_red->setObjectName("akt_red");

    akt_blue = new Rate(this);
	akt_blue->setFrameShape(QFrame::Box);
	//akt_blue->id = 2;
	//akt_blue->setObjectName("akt_blue");

	fam_red = new Fam(col_red, "", 63, "", this);
	//fam_red->setObjectName("fam_red");
	reg_red = new Fam(col_red, "", 10, "Lucida Console");
	//reg_red->setObjectName("reg_red");

	fam_blue = new Fam(col_blue, "", 63, "", this);
	//fam_blue->setObjectName("fam_blue");
	reg_blue = new Fam(col_blue, "", 10, "Lucida Console");
	//reg_blue->setObjectName("reg_blue");

    fam_next_red = new Fam(col_red, "", 63, "", this);
    fam_next_blue = new Fam(col_blue, "", 63, "", this);

	plus_red = new Plus(col_red, this);
    //plus_red->setObjectName("sd_plus_red");
	plus_blue = new Plus(col_blue, this);
    //plus_blue->setObjectName("sd_plus_blue");

    sec = new LCDTimer(this);
	//sec->setObjectName("sec");

    sec_doctor = new LCDStopwatch(this, "2:00", QColor(255, 255, 0), QColor(255, 255, 0), false, true);
    //sec_red->setObjectName("sd_sec_red");
    sec_doctor->hide();

    sec_red = new LCDStopwatch(this, "0:20", QColor(255, 0, 0), QColor(255, 102, 102), false, true);
    //sec_red->setObjectName("sd_sec_red");
	sec_red->hide();

    sec_blue = new LCDStopwatch(this, "0:20", QColor(0, 0, 255), QColor(102, 102, 255), false, true);
	//sec_blue->setObjectName("sec_blue");
	sec_blue->hide();

    sec_red_t = new LCDStopwatch(this, "2:00", QColor(255, 0, 0), QColor(255, 102, 102), false, true);
    sec_red_t->hide();

    sec_blue_t = new LCDStopwatch(this, "2:00", QColor(0, 0, 255), QColor(102, 102, 255), false, true);
    sec_blue_t->hide();

	np_red = new NP();
	//np_red->setObjectName("np_red");
	//np_red->sbros();
	np_blue = new NP();
	//np_blue->setObjectName("np_blue");
	//np_blue->sbros();

	ListFamily * lf = new ListFamily(this);
	lf->setObjectName("lf");

    cat = new QPushButton(this);//("yellow", this);
	cat->setObjectName("cat");
	//cat->setText("55");
	cat->setAutoFillBackground(true);
    cat->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    cat->setStyleSheet("background-color: black; color: white; text-align: center; border-radius: 20px");

    age = new QPushButton(this);//("yellow", this);
    age->setObjectName("age");
    age->setAutoFillBackground(true);
    age->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    age->setStyleSheet("background-color: black; color: white; text-align: center; border-radius: 20px");

	flag_blue = new QLabel(this);
	flag_blue->setScaledContents(true);
	//flag_blue->setAutoFillBackground(true);

	flag_red = new QLabel(this);
	flag_red->setScaledContents(true);
	//flag_red->setAutoFillBackground(true);

    //rV = new ReplayViewer(this);

	grid = new QGridLayout(this);
	grid->setObjectName("grid");
	//grid->setSpacing(6);
	//grid->setMargin(6);
    grid->addWidget(fam_blue,    0, 0, 6, 34);
    grid->addWidget(fam_red,    0, 34, 6, 34);

    grid->addWidget(reg_blue,   6, 0, 5, 34);
    grid->addWidget(reg_red,   6, 34, 5, 34);

    grid->addWidget(ball_blue,  11, 0, 18, 24);
    grid->addWidget(ball_red,  11, 44, 18, 24);
    grid->addWidget(akt_blue,  29, 5, 12, 14);
    grid->addWidget(akt_red,  29, 49, 12, 14);

    grid->addWidget(np_blue,    32, 21, 6, 6);
    grid->addWidget(np_red,     32, 41, 6, 6);

    grid->addWidget(plus_blue,   11,  0, 6, 6);
    grid->addWidget(plus_red,    11, 44, 6, 6);

    grid->addWidget(age,        29, 28, 6, 12);
    grid->addWidget(cat,        35, 28, 6, 12);

    grid->addWidget(sec,        13, 25, 14, 18);
    grid->addWidget(sec_doctor, 13, 25, 14, 18);
    grid->addWidget(sec_blue,   13,  3, 14, 18);
    grid->addWidget(sec_red,    13, 47, 14, 18);
    grid->addWidget(sec_blue_t, 13,  3, 14, 18);
    grid->addWidget(sec_red_t,  13, 47, 14, 18);

    grid->addWidget(lblNextBlue,   36, 0,  5, 5);
    grid->addWidget(lblNextRed,    36,  63,  5, 5);

    grid->addWidget(fam_next_blue,    41, 0, 4, 34);
    grid->addWidget(fam_next_red,    41, 34, 4, 34);

    //grid->addWidget(reg_blue,   37, 0, 5, 34);
    //grid->addWidget(reg_red,   37, 34, 5, 34);

    //grid->addWidget(rV, 0, 0, 42, 68);
    //grid->addWidget(rV, 37, 0, 5, 68);
    //rV->setGeometry(0, 0, width(), height());
    //rV->raise();
    showFullScreen();

	minimum_height_family = (height() - 12) / 42;
	minimum_height_region = minimum_height_family;

    //rV->setStyleSheet("z-index: -10");
    //rV->setFrameStyle(QFrame::Box);
}

TVScreen::~TVScreen()
{
}

void TVScreen::paintEvent(QPaintEvent * ) {
	QPainter pn;
	pn.begin(this);
	if (View == 0) {
		pn.setBrush(Qt::blue);
		pn.drawRect(0, 0, width() / 2, height());
		pn.setBrush(Qt::red);
		pn.drawRect(width() / 2, 0, width() / 2, height());
	}
	else {
		pn.setBrush(Qt::black);
		pn.drawRect(0, 0, width(), height());
	}
	pn.end();
}

void TVScreen::showEvent(QShowEvent *){
    /*
	minimum_height = static_cast<int>((height() - 12) / 42);
	qDebug() << "minimum_height = " << minimum_height;
	percent_height = static_cast<int>((height() - 12) / 100);
	qDebug() << "percent_height = " << percent_height;
	grid->setRowMinimumHeight(0, minimum_height);
	grid->setRowMinimumHeight(1, minimum_height);
	grid->setRowMinimumHeight(2, minimum_height);
	grid->setRowMinimumHeight(3, minimum_height);
	grid->setRowMinimumHeight(4, minimum_height);
	grid->setRowMinimumHeight(5, minimum_height);
	grid->setRowMinimumHeight(37, minimum_height);
	grid->setRowMinimumHeight(38, minimum_height);
	grid->setRowMinimumHeight(39, minimum_height);
	grid->setRowMinimumHeight(40, minimum_height);
	grid->setRowMinimumHeight(41, minimum_height);
    */
}

void TVScreen::resizeEvent(QResizeEvent *){
    //rV->setGeometry(0, 0, width(), height());

    minimum_height = (height() - 12) / 42;
    percent_height = (height() - 12) / 100;
    grid->setRowMinimumHeight(0, minimum_height);
    grid->setRowMinimumHeight(1, minimum_height);
    grid->setRowMinimumHeight(2, minimum_height);
    grid->setRowMinimumHeight(3, minimum_height);
    grid->setRowMinimumHeight(4, minimum_height);
    grid->setRowMinimumHeight(5, minimum_height);
    grid->setRowMinimumHeight(6, minimum_height);
    grid->setRowMinimumHeight(7, minimum_height);
    grid->setRowMinimumHeight(8, minimum_height);
    grid->setRowMinimumHeight(9, minimum_height);
    grid->setRowMinimumHeight(10, minimum_height);
    grid->setRowMinimumHeight(41, minimum_height);
    grid->setRowMinimumHeight(42, minimum_height);
    grid->setRowMinimumHeight(43, minimum_height);
    grid->setRowMinimumHeight(44, minimum_height);
    QFont f;
    f.setPixelSize(cat->height() * 1.5);
    //cat->setFont(f);
}




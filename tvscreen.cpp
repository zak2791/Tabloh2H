
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
#include <QPicture>

TVScreen::TVScreen(QWidget * parent) : QWidget(parent) {

    SvgLabel* lblNextRed = new SvgLabel(":/images/next.svg");

    lblNextRed->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

    SvgLabel* lblNextBlue = new SvgLabel(":/images/next.svg");

    lblNextBlue->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

	col_red = "white";
	col_blue = "white";

	View = 0;			//красно-синий фон

    ball_red = new Rate(this);
	ball_red->setFrameShape(QFrame::Box);


    ball_blue = new Rate(this);
	ball_blue->setFrameShape(QFrame::Box);

    fam_red = new FirstLastName(col_red, "", 63, "", this);

	reg_red = new Fam(col_red, "", 10, "Lucida Console");

    fam_blue = new FirstLastName(col_blue, "", 63, "", this);

	reg_blue = new Fam(col_blue, "", 10, "Lucida Console");

    fam_next_red = new Fam(col_red, "", 63, "", this);
    fam_next_blue = new Fam(col_blue, "", 63, "", this);

	plus_red = new Plus(col_red, this);
    plus_red->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

	plus_blue = new Plus(col_blue, this);
    plus_blue->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    sec = new LCDTimer(this);

    sec_doctor = new LCDStopwatch(this, "2:01", QColor(255, 255, 0), QColor(255, 255, 0), false, true);

    sec_doctor->hide();

    sec_red = new LCDStopwatch(this, "0:20", QColor(255, 0, 0), QColor(255, 102, 102), false, true);

	sec_red->hide();

    sec_blue = new LCDStopwatch(this, "0:20", QColor(0, 0, 255), QColor(102, 102, 255), false, true);

	sec_blue->hide();

    sec_red_t = new LCDStopwatch(this, "2:01", QColor(255, 0, 0), QColor(255, 102, 102), false, true);
    sec_red_t->hide();

    sec_blue_t = new LCDStopwatch(this, "2:01", QColor(0, 0, 255), QColor(102, 102, 255), false, true);
    sec_blue_t->hide();

	np_red = new NP();

	np_blue = new NP();

    cat = new QLabel(this);//("yellow", this);
	cat->setObjectName("cat");

    cat->setAlignment(Qt::AlignCenter);

    cat->setStyleSheet("font-weight: bold; color: white;");

    age = new QLabel(this);//("yellow", this);
    age->setObjectName("age");

    age->setAlignment(Qt::AlignCenter);

    age->setStyleSheet("font-weight: bold; color: white;");

	flag_blue = new QLabel(this);
	flag_blue->setScaledContents(true);

	flag_red = new QLabel(this);
    flag_red->setScaledContents(true);

    QHBoxLayout* hbb = new QHBoxLayout;
    hbb->addWidget(np_blue);

    QHBoxLayout* hbr = new QHBoxLayout;
    hbr->addWidget(np_red);

    player = new PlayerViewerTV;
    //vWidget = new VideoWidget(this);

	grid = new QGridLayout(this);
	grid->setObjectName("grid");

    grid->addWidget(fam_blue,    0, 0, 11, 34);
    grid->addWidget(fam_red,    0, 34, 11, 34);

    grid->addWidget(reg_blue,   11, 0, 5, 34);
    grid->addWidget(reg_red,   11, 34, 5, 34);

    grid->addWidget(ball_blue,  16,  0, 18, 24);
    grid->addWidget(ball_red,   16, 44, 18, 24);

    grid->addLayout(hbb, 34,  5, 7, 13);

    grid->addLayout(hbr, 34, 50, 7, 13);

    grid->addWidget(plus_blue,   22,  1, 6, 6);
    grid->addWidget(plus_red,    22, 45, 6, 6);

    grid->addWidget(age,        34, 18, 7, 16);
    grid->addWidget(cat,        34, 34, 7, 16);

    grid->addWidget(sec,        18, 25, 14, 18);
    grid->addWidget(sec_doctor, 18, 25, 14, 18);
    grid->addWidget(sec_blue,   20,  3, 10, 18);
    grid->addWidget(sec_red,    20, 47, 10, 18);
    grid->addWidget(sec_blue_t, 20,  3, 10, 18);
    grid->addWidget(sec_red_t,  20, 47, 10, 18);

    grid->addWidget(lblNextBlue,   34,   0,  7, 5);
    grid->addWidget(lblNextRed,    34,  63,  7, 5);

    grid->addWidget(fam_next_blue,    41, 0, 4, 34);
    grid->addWidget(fam_next_red,    41, 34, 4, 34);

    logo = new logofrb(this);


    grid->addWidget(logo,    16, 25, 18, 18);

    lblEndTimer = new EndTime(this);

	minimum_height_family = (height() - 12) / 42;
	minimum_height_region = minimum_height_family;

    setWindowTitle("TabloOnTV");

    connect(this, SIGNAL(sigLogo(bool)), logo, SLOT(setLogo(bool)));

}

TVScreen::~TVScreen()
{
}

void TVScreen::showPlayer(){
    if(flagReplayOnTV){
        grid->addWidget(player,    0, 0, 45, 68);
        player->show();
    }
}

void TVScreen::showPlayer2(){
    if(flagReplayOnTV){
        //grid->addWidget(vWidget,    0, 0, 45, 68);
        //vWidget->show();
    }
}

void TVScreen::setPlayerEnabled(bool b){
    flagReplayOnTV = b;
}

void TVScreen::hidePlayer(){
    grid->removeWidget(player);
    player->hide();
    //grid->removeWidget(vWidget);
    //vWidget->hide();
}

void TVScreen::hidePlayer2(){
    qDebug()<<"hhhhhhhhhhhh";
    //grid->removeWidget(vWidget);
    //vWidget->hide();
    //vWidget->setVisible(false);
    //qDebug()<<vWidget->isVisible();
    //repaint();
}

void TVScreen::paintEvent(QPaintEvent * ) {
    QPainter pn;
    pn.begin(this);

    pn.setBrush(Qt::blue);
    pn.drawRect(0, 0, width() / 2, height());
    pn.setBrush(Qt::red);
    pn.drawRect(width() / 2, 0, width() / 2, height());

    pn.end();
}

void TVScreen::resizeEvent(QResizeEvent *){

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
    grid->setRowMinimumHeight(11, minimum_height);
    grid->setRowMinimumHeight(12, minimum_height);
    grid->setRowMinimumHeight(13, minimum_height);
    grid->setRowMinimumHeight(14, minimum_height);
    grid->setRowMinimumHeight(15, minimum_height);
    grid->setRowMinimumHeight(41, minimum_height);
    grid->setRowMinimumHeight(42, minimum_height);
    grid->setRowMinimumHeight(43, minimum_height);
    grid->setRowMinimumHeight(44, minimum_height);

    lblEndTimer->setGeometry(0, 0, width(), height());

}

void TVScreen::mouseDoubleClickEvent(QMouseEvent*)
{
    if(windowState() == Qt::WindowFullScreen)
        showNormal();
    else
        showFullScreen();
}




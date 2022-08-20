#include <QPainter>
#include <QGridLayout>
#include <QPushButton>
#include <QEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include "filter.h"
#include <QMessageBox>
#include <QPalette>
#include <QProcess>

#include "First_display.h"
#include "replayviewer.h"

#include <math.h>


#include "list_family.h"
#include "category.h"

//#include <QDir>


class MyEvent : public QEvent {
public:
	MyEvent(int ev) : QEvent{ (Type)(QEvent::User + ev) } {}

};

FirstDisplay::FirstDisplay(QWidget * parent) : QWidget(parent){
	ev_L = new MyEvent(200);
	ev_R = new MyEvent(201);

	col_red = "white";
	col_blue = "white";

	HEIGHT_REGION = 0;
	HEIGHT_FAMILY = 0;
	minimum_height = 0;
	percent_height = 0;

    View = 0;			//красно-синий фон

	ball_red = new Ball(this, 99, false);
	ball_red->setFrameShape(QFrame::Box);
	ball_red->id = 1;
	ball_red->setObjectName("ball_red");

	ball_blue = new Ball(this, 99, false);
	ball_blue->setFrameShape(QFrame::Box);
	ball_blue->id = 0;
	ball_blue->setObjectName("ball_blue");

	akt_red = new Ball(this, 99, true);
	akt_red->setFrameShape(QFrame::Box);
	akt_red->id = 3;
	akt_red->setObjectName("akt_red");

	akt_blue = new Ball(this, 99, true);
	akt_blue->setFrameShape(QFrame::Box);
	akt_blue->id = 2;
	akt_blue->setObjectName("akt_blue");

	fam_red = new Fam(col_red, "", 63,"",this);
	fam_red->setObjectName("fam_red");
	reg_red = new Fam(col_red, "", 10, "Lucida Console");
	reg_red->setObjectName("reg_red");

	fam_blue = new Fam(col_blue, "", 63,"",this);
	fam_blue->setObjectName("fam_blue");
	reg_blue = new Fam(col_blue, "", 10, "Lucida Console");
	reg_blue->setObjectName("reg_blue");
	
	QPushButton * btnParter_red = new QPushButton(u8"ПАРТЕР", this);
	btnParter_red->setObjectName("btnParter_red");
	btnParter_red->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	btnParter_red->setStyleSheet("color: red; font: bold " + QString::number(round(btnParter_red->height() / 2)) + "px;");
	btnParter_red->setFocusPolicy(Qt::NoFocus);
	
	QPushButton * btnTime = new QPushButton(u8"ВРЕМЯ", this);
	btnTime->setObjectName("btnTime");
	btnTime->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	//btnTime->setStyleSheet("color: green; font: bold " + QString::number(round(btnTime->height() / 2)) + "px;");
	btnTime->setFocusPolicy(Qt::NoFocus);

	QPushButton * btnParter_blue = new QPushButton(u8"ПАРТЕР", this);
	btnParter_blue->setObjectName("btnParter_blue");
	btnParter_blue->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	btnParter_blue->setStyleSheet("color: blue; font: bold " + QString::number(round(btnParter_blue->height() / 2)) + "px;");
	btnParter_blue->setFocusPolicy(Qt::NoFocus);
	
	QPushButton * btnTehTime_red = new QPushButton(u8"Т.ВРЕМЯ", this);
	btnTehTime_red->setObjectName("btnTehTime_red");
	btnTehTime_red->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	btnTehTime_red->setStyleSheet("color: red; font: bold " + QString::number(round(btnTehTime_red->height() / 2)) + "px;");
	btnTehTime_red->setFocusPolicy(Qt::NoFocus);

	QPushButton * btnSettings = new QPushButton(u8"НАСТРОЙКИ", this);
	btnSettings->setObjectName("btnSettings");
	btnSettings->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	btnSettings->setStyleSheet("color: green; font: bold " + QString::number(round(btnSettings->height() / 2)) + "px;");
	btnSettings->setFocusPolicy(Qt::NoFocus);
	
	QPushButton * btnTehTime_blue = new QPushButton(u8"Т.ВРЕМЯ", this);
	btnTehTime_blue->setObjectName("btnTehTime_blue");
	btnTehTime_blue->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	btnTehTime_blue->setStyleSheet("color: blue; font: bold " + QString::number(round(btnTehTime_blue->height() / 2)) + "px;");
	btnTehTime_blue->setFocusPolicy(Qt::NoFocus);

	QPushButton * btnPlus_red = new QPushButton("+", this);
	btnPlus_red->setObjectName("btnPlus_red");
	btnPlus_red->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	btnPlus_red->setStyleSheet("color: red; font: bold " + QString::number(btnPlus_red->height()) + "px;");
	btnPlus_red->setFocusPolicy(Qt::NoFocus);

	QPushButton * btnPlus_blue = new QPushButton("+", this);
	btnPlus_blue->setObjectName("btnPlus_blue");
	btnPlus_blue->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	btnPlus_blue->setStyleSheet("color: blue; font: bold " + QString::number(btnPlus_blue->height()) + "px;");
	btnPlus_blue->setFocusPolicy(Qt::NoFocus);

	QPushButton * btnView = new QPushButton(u8"ВИД", this);
	//btnView->setObjectName("btnPlus_blue");
	btnView->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	btnView->setStyleSheet("font: bold " + QString::number(round(btnView->height() / 2)) + "px;");
	btnView->setFocusPolicy(Qt::NoFocus);

	plus_red = new Plus(col_red, this);
	plus_red->setObjectName("plus_red");
	plus_blue = new Plus(col_blue, this);
	plus_blue->setObjectName("plus_blue");

	sec = new Secundomer(this, ev_L, ev_R, "chartreuse", "green", 1.30, -1, false, false);
	sec->setObjectName("sec");

	Secundomer * sec_red = new Secundomer(this, ev_L, ev_R, "salmon", "red", 0.20, 1, true, false);
	sec_red->setObjectName("sec_red");
	sec_red->hide();

	Secundomer * sec_blue = new Secundomer(this, ev_L, ev_R, "lightblue", "blue", 0.20, 1, true, false);
	sec_blue->setObjectName("sec_blue");
	sec_blue->hide();

    Secundomer * sec_red_t = new Secundomer(this, ev_L, ev_R, "salmon", "red", 2.00, 1, true, false);
    sec_red_t->setObjectName("sec_red_t");
    sec_red_t->hide();

    Secundomer * sec_blue_t = new Secundomer(this, ev_L, ev_R, "lightblue", "blue", 2.00, 1, true, false);
    sec_blue_t->setObjectName("sec_blue_t");
    sec_blue_t->hide();


	np_red = new NP();
	np_red->setObjectName("np_red");
	//np_red->sbros();
	np_blue = new NP();
	np_blue->setObjectName("np_blue");
	//np_blue->sbros();

	nv_red = new NV();
	nv_red->setObjectName("nv_red");
	//nv_red->sbros();
	nv_blue = new NV();
	nv_blue->setObjectName("nv_blue");
    //nv_blue->setFocusPolicy(Qt::NoFocus);

    ListFamily * lf = new ListFamily(this);
	lf->setObjectName("lf");
	
    cat = new Category("yellow", this);
	cat->setObjectName("cat");
	//cat->setText("55");
	cat->setAutoFillBackground(true);
	cat->setStyleSheet("background-color: black");

	formView = new QWidget;
	//Ui::frmView ui;
	ui.setupUi(formView);

    frmTime = new QWidget;
    uiTime.setupUi(frmTime);
    //frmTime->setObjectName("setTime");

    //QWidget * rV_w = new QWidget();
    rV = new ReplayViewer();
    //rV_w->setStyleSheet("background-color: rgb(0,0,0)");
    //rV->setObjectName("rV");

    connect(btnView, SIGNAL(clicked(bool)), this, SLOT(showView()));

	connect(ui.rbView1, SIGNAL(toggled(bool)), this, SLOT(setView(void)));
	connect(ui.rbView2, SIGNAL(toggled(bool)), this, SLOT(setView(void)));
	connect(ui.rbView3, SIGNAL(toggled(bool)), this, SLOT(setView(void)));
	connect(ui.sbFrame, SIGNAL(valueChanged(int)), this, SLOT(setFrameWidth(int)));
	connect(ui.sbSpacing, SIGNAL(valueChanged(int)), this, SLOT(setSpace(int)));
	connect(ui.sbSec, SIGNAL(valueChanged(int)), this, SLOT(setSec(int)));

    connect(ui.cbCam1, SIGNAL(stateChanged(int)), this, SLOT(turnCamera(int)));
    connect(ui.cbCam2, SIGNAL(stateChanged(int)), this, SLOT(turnCamera(int)));
	
	flag_blue = new QLabel(this);
	flag_blue->setScaledContents(true);
	//flag_blue->setAutoFillBackground(true);

	flag_red = new QLabel(this);
	flag_red->setScaledContents(true);
	//flag_red->setAutoFillBackground(true);

    QWidget * cv_left_w = new QWidget(this);
    cv_left = new CameraViewer(cv_left_w);

    QWidget * cv_right_w = new QWidget(this);
    cv_right = new CameraViewer(cv_right_w);

    btnStopRecord = new QPushButton("Стоп\nзапись");
    btnStopRecord->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    connect(btnStopRecord, SIGNAL(clicked()), SLOT(StopRecord()));

    btnStartPlay = new QPushButton("Просмотр");
    btnStartPlay->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    connect(btnStartPlay, SIGNAL(clicked()), SLOT(PlayFile()));

	grid = new QGridLayout(this);
	grid->setObjectName("grid");
	//spacing = 6;
	//margin = 6;
    //сетка 68х42
	grid->setSpacing(6);
	grid->setMargin(6);
	grid->addWidget(fam_red,		 0,  0,  6,  34);
	grid->addWidget(fam_blue,		 0,  34, 6,  34);

	grid->addWidget(ball_red,		 6,  0,  18, 24);
	grid->addWidget(ball_blue,		 6,  44, 18, 24);
	grid->addWidget(akt_red,		 24, 0,  13, 14);
	grid->addWidget(akt_blue,		 24, 54, 13, 14);

	grid->addWidget(np_red,			 24, 19, 5,  5);
	grid->addWidget(np_blue,		 24, 44, 5,  5);

	grid->addWidget(nv_red,			 24, 14, 5,  5);
	grid->addWidget(nv_blue,		 24, 49, 5,  5);

	grid->addWidget(btnTehTime_red,  6,  24, 2,  6);
	grid->addWidget(btnSettings,	 6,  30, 2,  8);
	grid->addWidget(btnTehTime_blue, 6,  38, 2,  6);

	grid->addWidget(btnPlus_red,	 8,  30, 2,  3);
	grid->addWidget(btnPlus_blue,	 8,  35, 2,  3);

    grid->addWidget(plus_red,		 8,  24, 4,  4);
    grid->addWidget(plus_blue,		 8,  40, 4,  4);

    grid->addWidget(cat,			 20, 31, 2,  6);

    grid->addWidget(btnView,		 20, 24, 2,  6);

	grid->addWidget(btnParter_red,   22, 24, 2,  6);
	grid->addWidget(btnTime,		 22, 31, 2,  6);
	grid->addWidget(btnParter_blue,  22, 38, 2,  6);

    grid->addWidget(btnStopRecord,   20, 41, 2,  3);
    grid->addWidget(btnStartPlay,    20, 38, 2,  3);

	grid->addWidget(sec,			 24, 24, 13, 20);

	grid->addWidget(sec_red,		 9,  0,  12, 24);
	grid->addWidget(sec_blue,		 9,  44, 12, 24);

    grid->addWidget(sec_red_t,		 9,  0,  12, 24);
    grid->addWidget(sec_blue_t,		 9,  44, 12, 24);

	grid->addWidget(flag_red,		 29, 14, 8,  10);
	grid->addWidget(flag_blue,		 29, 44, 8,  10);
	
    grid->addWidget(reg_red,		 37, 0,  5,  34);
    grid->addWidget(reg_blue,		 37, 34, 5,  34);

    grid->addWidget(cv_left_w,		 13, 24, 6,  10);
    grid->addWidget(cv_right_w,		 13, 34, 6,  10);

    grid->addWidget(rV,              0, 13,  23,  42);

    connect(rV, SIGNAL(end_replay()), SLOT(removeReplay()));
    rV->hide();
    //minimum_height_family = (height() - 12) / 42;
	//minimum_height_region = minimum_height_family;

    btnTime->setStyleSheet("color: green; font: bold " + QString::number(round(btnTime->height() / 2)) + "px;");

    //desk = QApplication::desktop();

    //if (desk->numScreens() == 2){
    if(QGuiApplication::screens().count() == 2){
        showFullScreen();
    }else{
        show();
    }

    //if (desk->numScreens() == 1) {
    if(QGuiApplication::screens().count() == 1){
        QMessageBox::information(this, u8"ВНИМАНИЕ!",
        u8"Подключите к компьютеру дисплей в режиме \"Расширенный рабочий стол!\"",
		QMessageBox::Ok);
	}
    //else {
		SD = new SecondDisplay();

    //if (desk->numScreens() == 1)
    if(QGuiApplication::screens().count() == 1)
        SD->setGeometry(0, 0, QApplication::desktop()->availableGeometry(this).width() / 2, QApplication::desktop()->availableGeometry(this).height() / 2);
    else{
        SD->setGeometry(width(), 0, 100, height());
        SD->setGeometry(QApplication::desktop()->availableGeometry(this).right(),
                        0, QApplication::desktop()->availableGeometry(SD).width(),
                        QApplication::desktop()->availableGeometry(SD).height());
    }

		SD->show();

		connect(ball_red,	SIGNAL(ball(int)),		SD->ball_red,	SLOT(setBall(int)));
		connect(ball_blue,	SIGNAL(ball(int)),		SD->ball_blue,	SLOT(setBall(int)));
		connect(akt_red,	SIGNAL(ball(int)),		SD->akt_red,	SLOT(setBall(int)));
		connect(akt_blue,	SIGNAL(ball(int)),		SD->akt_blue,	SLOT(setBall(int)));

		connect(np_red,		SIGNAL(ball(QString)),	SD->np_red,		SLOT(setValue(QString)));
		connect(np_blue,	SIGNAL(ball(QString)),	SD->np_blue,	SLOT(setValue(QString)));
		connect(nv_red,		SIGNAL(ball(QString)),	SD->nv_red,		SLOT(setValue(QString)));
		connect(nv_blue,	SIGNAL(ball(QString)),	SD->nv_blue,	SLOT(setValue(QString)));

        connect(plus_red,   SIGNAL(textChange(QString)), SD->plus_red, SLOT(setData(QString)));
        connect(plus_blue,  SIGNAL(textChange(QString)), SD->plus_blue, SLOT(setData(QString)));

        connect(fam_red,  SIGNAL(sigText(QString)), SD->fam_red, SLOT(Text(QString)));
        connect(fam_blue,  SIGNAL(sigText(QString)), SD->fam_blue, SLOT(Text(QString)));
        connect(reg_red,  SIGNAL(sigText(QString)), SD->reg_red, SLOT(Text(QString)));
        connect(reg_blue,  SIGNAL(sigText(QString)), SD->reg_blue, SLOT(Text(QString)));

        connect(cat,  SIGNAL(sigText(QString)), SD->cat, SLOT(setData(QString)));

        connect(sec,  SIGNAL(sec_sig(QString, QPalette)), SD->sec, SLOT(doubleTime(QString, QPalette)));

        //connect(btnSettings, SIGNAL(clicked()), this, SIGNAL(setCategory()));

        connect(uiTime.dMin, SIGNAL(valueChanged(int)), this, SLOT(setTime()));
        connect(uiTime.dSec, SIGNAL(valueChanged(int)), this, SLOT(setTime()));
        connect(uiTime.dSec2, SIGNAL(valueChanged(int)), this, SLOT(setTime()));


    //}

    camLeftThread = new FFmpegThread(1);
    camLeftThread->setObjectName("camTread1");
    connect(camLeftThread, SIGNAL(imgView(QImage)), cv_left, SLOT(draw_image (QImage)));

    cam1Thread = new QThread(this);
    camLeftThread->moveToThread(cam1Thread);
    connect(cam1Thread, SIGNAL(started()), camLeftThread, SLOT(turn_on()));
    connect(camLeftThread, SIGNAL(err()), this, SLOT(errorCamera()));
    //connect(camLeftThread, SIGNAL(imgReplay(QImage, int)), SD->rV, SLOT(draw_image (QImage, int)));
    connect(camLeftThread, SIGNAL(imgReplay(QImage, int)), rV, SLOT(draw_image (QImage, int)));

    camRightThread = new FFmpegThread(2);
    camRightThread->setObjectName("camTread2");
    connect(camRightThread, SIGNAL(imgView(QImage)), cv_right, SLOT(draw_image (QImage)));

    cam2Thread = new QThread(this);
    camRightThread->moveToThread(cam2Thread);
    connect(cam2Thread, SIGNAL(started()), camRightThread, SLOT(turn_on()));
    connect(camRightThread, SIGNAL(err()), this, SLOT(errorCamera()));
    //connect(camRightThread, SIGNAL(imgReplay(QImage, int)), SD->rV, SLOT(draw_image (QImage, int)));
    connect(camRightThread, SIGNAL(imgReplay(QImage, int)), rV, SLOT(draw_image (QImage, int)));

    //camLeftThread->start();
    connect(ui.leCam1, SIGNAL(editingFinished()), this, SLOT(setCam()));
    connect(ui.leCam2, SIGNAL(editingFinished()), this, SLOT(setCam()));

    connect(sec_red, SIGNAL(sec_visible(bool)), SD->sec_red, SLOT(setVisible(bool)));
    connect(sec_red,  SIGNAL(sec_sig(QString, QPalette)), SD->sec_red, SLOT(doubleTime(QString, QPalette)));

    connect(sec_blue, SIGNAL(sec_visible(bool)), SD->sec_blue, SLOT(setVisible(bool)));
    connect(sec_blue,  SIGNAL(sec_sig(QString, QPalette)), SD->sec_blue, SLOT(doubleTime(QString, QPalette)));

    connect(sec_red_t, SIGNAL(sec_visible(bool)), SD->sec_red, SLOT(setVisible(bool)));
    connect(sec_red_t,  SIGNAL(sec_sig(QString, QPalette)), SD->sec_red, SLOT(doubleTime(QString, QPalette)));

    connect(sec_blue_t, SIGNAL(sec_visible(bool)), SD->sec_blue, SLOT(setVisible(bool)));
    connect(sec_blue_t,  SIGNAL(sec_sig(QString, QPalette)), SD->sec_blue, SLOT(doubleTime(QString, QPalette)));


    connect(sec, SIGNAL(startRec()), camLeftThread, SLOT(startRecord()), Qt::DirectConnection);
    connect(sec, SIGNAL(stopRec()),  camLeftThread, SLOT(stopRecord()), Qt::DirectConnection);
    connect(sec, SIGNAL(startRec()), camRightThread, SLOT(startRecord()), Qt::DirectConnection);
    connect(sec, SIGNAL(stopRec()), camRightThread, SLOT(stopRecord()), Qt::DirectConnection);

    connect(ui.cbReplay, SIGNAL(stateChanged(int)), this, SLOT(enableReplayOnTV(int)));

    connect(ui.rb3s, SIGNAL(toggled(bool)), SLOT(setTimeReplay(bool)));
    //connect(ui.rb5s, SIGNAL(toggled()), SLOT(setTimeReplay()));

    connect(ui.rb3s, SIGNAL(toggled(bool)), SLOT(setTimeReplay(bool)));
    //connect(ui.rb5s, SIGNAL(toggled()), SLOT(setTimeReplay()));


    QFile cam1("cam1.txt");
    if(!cam1.exists()){
        cam1.open(QIODevice::WriteOnly);
        cam1.close();
    }

    QFile cam2("cam2.txt");
    if(!cam2.exists()){
        cam2.open(QIODevice::WriteOnly);
        cam2.close();
    }

    //KeyFilter kf(this);
    //this->installEventFilter(&kf);
    //pwgt->installEventFilter(&kf);
    QList<QWidget*> widgetList = this->findChildren<QWidget*>();
    foreach( QWidget* widgetPtr, widgetList )
        widgetPtr->setFocusPolicy(Qt::NoFocus);
    this->setFocus();

    //SD->setObjectName("sd");

    //connect(this, SIGNAL(signalReplay(void)), camLeftThread, SLOT(start_replay(void)));
    //connect(this, SIGNAL(signalReplay()), camLeftThread, SLOT(startReplay()));

    QFile f("start.txt");
    if(!f.exists()){
       f.open(QIODevice::WriteOnly);
       f.close();
    }else{
        f.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream in(&f);
        int i = 1;
        while (!in.atEnd()) {
            QString line = in.readLine();
            process_line(i++, line);
        }
        f.close();
    }
}

void FirstDisplay::StopRecord(){
    //qDebug()<<"StopRecord";
    camLeftThread->stopRecord();
}

void FirstDisplay::PlayFile(){
    qDebug()<<"PlayFile";
    QProcess proc;
    QStringList l;
    l.append(QString("-fs"));
    l.append(camLeftThread->playFile);
    proc.start("ffplay", l);
    proc.waitForFinished(-1);
    qDebug()<<"code = "<<proc.exitCode();
}

void FirstDisplay::enableReplayOnTV(int state){
    if(!state){
        disconnect(camLeftThread, SIGNAL(imgReplay(QImage, int)), SD->rV, nullptr);
        disconnect(camRightThread, SIGNAL(imgReplay(QImage, int)), SD->rV, nullptr);
    }else{
        connect(camLeftThread, SIGNAL(imgReplay(QImage, int)), SD->rV, SLOT(draw_image (QImage, int)));
        connect(camRightThread, SIGNAL(imgReplay(QImage, int)), SD->rV, SLOT(draw_image (QImage, int)));
    }
}

void FirstDisplay::setTimeReplay(bool state){
    if(state){
        camLeftThread->setTimeReplay(3);
        camRightThread->setTimeReplay(3);
    }else{
        camLeftThread->setTimeReplay(5);
        camRightThread->setTimeReplay(5);
    }
}

void FirstDisplay::removeReplay(){
    rV->hide();
}

void FirstDisplay::showReplay(){
    rV->show();
}

void FirstDisplay::process_line(int i, QString s){
    switch (i) {
    case 1:
        fam_red->Text(s);
        break;
    case 2:
        fam_blue->Text(s);
        break;
    case 3:
        ball_red->setBall(s.toInt());
        break;
    case 4:
        ball_blue->setBall(s.toInt());
        break;
    case 5:
        if(s == "1")
            plus_red->setData("+");
        if(s == "2")
            plus_blue->setData("+");
        break;
    case 6:
        cat->setData(s);
        break;
    case 7:
        akt_red->setBall(s.toInt());
        break;
    case 8:
        akt_blue->setBall(s.toInt());
        break;
    case 9:
        nv_red->setValue(s);
        SD->nv_red->setValue(s);
        break;
    case 10:
        np_red->setValue(s);
        SD->np_red->setValue(s);
        break;
    case 11:
        np_blue->setValue(s);
        SD->np_blue->setValue(s);
        break;
    case 12:
        nv_blue->setValue(s);
        SD->nv_blue->setValue(s);
        break;
    case 13:
        sec->showLastTime(s);
        break;
    case 14:
        reg_red->Text(s);
        break;
    case 15:
        reg_blue->Text(s);
        break;
    }

}

FirstDisplay::~FirstDisplay(){
    ui.cbCam1->setCheckState(Qt::Unchecked);
    ui.cbCam2->setCheckState(Qt::Unchecked);
    QFile f("start.txt");
    f.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&f);
    out << fam_red->getText() << "\n";
    out << fam_blue->getText() << "\n";
    out << ball_red->text() << "\n";
    out << ball_blue->text() << "\n";
    if(plus_red->getText() == "+")
        out << "1" << "\n";
    else if(plus_blue->getText() == "+")
        out << "2" << "\n";
    else
        out << "0" << "\n";
    out << cat->text() << "\n";
    out << akt_red->text() << "\n";
    out << akt_blue->text() << "\n";
    out << nv_red->text() << "\n";
    out << np_red->text() << "\n";
    out << np_blue->text() << "\n";
    out << nv_blue->text() << "\n";
    out << sec->getLastTime() << "\n";
    out << reg_red->getText() << "\n";
    out << reg_blue->getText() << "\n";
}

void FirstDisplay::start_replay(){
    camLeftThread->startReplay();
    camRightThread->startReplay();
}

void FirstDisplay::setTime(){
    int min = uiTime.dMin->value();
    int sec1 = uiTime.dSec->value();
    int sec2 = uiTime.dSec2->value();
    if(min == 0 && sec1 == 0 && sec2 == 0)
        return;
    sec->setTime(min, sec1, sec2);
    qDebug()<<"setTime";
}

void FirstDisplay::showView(){
    QFile f1("cam1.txt");
    f1.open(QIODevice::ReadOnly);
    ui.leCam1->setText(f1.readLine());
    f1.close();
    QFile f2("cam2.txt");
    f2.open(QIODevice::ReadOnly);
    ui.leCam2->setText(f2.readLine());
    f2.close();
    formView->show();
}

void FirstDisplay::errorCamera(){
    qDebug()<<sender()->objectName();
    if(sender()->objectName() == "camTread1"){
        ui.cbCam1->setChecked(false);
    }else{
        ui.cbCam2->setChecked(false);
    }
}

void FirstDisplay::setCam(){
    QFile f;
    QTextStream out(&f);
    if(sender()->objectName() == "leCam1"){
        f.setFileName("cam1.txt");
        f.open(QIODevice::WriteOnly);
        out << ui.leCam1->text();
    }else{
        f.setFileName("cam2.txt");
        f.open(QIODevice::WriteOnly);
        out << ui.leCam2->text();
    }
    f.close();
}

void FirstDisplay::turnCamera(int state){

    if(state == 2){
        if(sender()->objectName() == "cbCam1"){
            qDebug()<<"cam1 on";
            QFile f("cam1.txt");
            f.open(QIODevice::ReadOnly);
            camLeftThread->setUrl(f.readLine());
            cam1Thread->start();
            f.close();
        }
        if(sender()->objectName() == "cbCam2"){
            qDebug()<<"cam2 on";
            QFile f("cam2.txt");
            f.open(QIODevice::ReadOnly);
            camRightThread->setUrl(f.readLine());
            cam2Thread->start();
            f.close();
        }
    }
    else{
        if(sender()->objectName() == "cbCam1"){
            camLeftThread->turn_off();
            cam1Thread->quit();
            cam1Thread->wait();
            qDebug()<<"errCamera1";
        }
        if(sender()->objectName() == "cbCam2"){
            camRightThread->turn_off();
            cam2Thread->quit();
            cam2Thread->wait();
            qDebug()<<"errCamera2";
        }
    }

}

void FirstDisplay::paintEvent(QPaintEvent * ) {
	QPainter pn;
	pn.begin(this);
	if (View == 0) {
		pn.setBrush(Qt::red);
		pn.drawRect(0, 0, width() / 2, height());
		pn.setBrush(Qt::blue);
		pn.drawRect(width() / 2, 0, width() / 2, height());
	}
	else {
		pn.setBrush(Qt::black);
		pn.drawRect(0, 0, width(), height());
	}
	pn.end();
}

void FirstDisplay::keyPressEvent(QKeyEvent * pe){
    //qDebug()<<pe;
	if (pe->key() == Qt::Key_Up)
		changeSize(0);
	else if (pe->key() == Qt::Key_Down)
		changeSize(1);
	else if (pe->key() == Qt::Key_Right)
		changeSize(2);
	else if (pe->key() == Qt::Key_Left)
		changeSize(3);
    else if (pe->key() == Qt::Key_F1){
        //setCategory();
        frmTime->show();
    }
	else {
        //qDebug() << "key = " << pe->key();
		sendKey(pe->key());
	}
}

void FirstDisplay::resizeEvent(QResizeEvent *){
    minimum_height = (height() - 12) / 42;

    percent_height = (height() - 12) / 100;
    int w = (width() - 12) * 4 / 68;
    //cv_left->setSize(w, w / 2);
    //cv_left->resize(w, w / 2);
}

void FirstDisplay::showEvent(QShowEvent *){
	minimum_height = (height() - 12) / 42;

	percent_height = (height() - 12) / 100;

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

}

void FirstDisplay::sendEV_L(QObject * pObj)
{
	QApplication::sendEvent(pObj, ev_L);
}

void FirstDisplay::sendEV_R(QObject * pObj)
{
	QApplication::sendEvent(pObj, ev_R);
}

void FirstDisplay::setFlagRed(QString s){
	QString st = "./flags/" + s + ".png";
	QFile * pFile = new QFile(st);
	if (pFile->exists() == true) {
		QPixmap flag = QPixmap();
		flag.load(st);
		flag = flag.scaled(flag_red->size());
		flag_red->setPixmap(flag);
	}
}

void FirstDisplay::setFlagBlue(QString s){
	QString st = "./flags/" + s + ".png";
	QFile * pFile = new QFile(st);
	if (pFile->exists() == true) {
		QPixmap flag = QPixmap();
		flag.load(st);
		flag = flag.scaled(flag_red->size());
		flag_blue->setPixmap(flag);
	}
}

void FirstDisplay::setView(void){
	if (ui.rbView1->isChecked()) {
		View = 0;
		ball_red->setViewStyle(0,  ui.sbFrame->value());
		ball_blue->setViewStyle(0, ui.sbFrame->value());
		akt_red->setViewStyle(0,   ui.sbFrame->value());
		akt_blue->setViewStyle(0,  ui.sbFrame->value());
		fam_red->setViewStyle(0);
		fam_blue->setViewStyle(0);
		reg_red->setViewStyle(0);
		reg_blue->setViewStyle(0);
		fam_red->align = 0;
		fam_blue->align = 0;
		reg_red->align = 0;
		reg_blue->align = 0;
		sec->setFrameShape(QFrame::Box);
		plus_red->setColor("white");
		plus_blue->setColor("white");
        //if (desk->numScreens() != 1) {
			SD->View = 0;
			SD->ball_red->setViewStyle(0,  ui.sbFrame->value());
			SD->ball_blue->setViewStyle(0, ui.sbFrame->value());
			SD->akt_red->setViewStyle(0,   ui.sbFrame->value());
			SD->akt_blue->setViewStyle(0,  ui.sbFrame->value());
			SD->fam_red->setViewStyle(0);
			SD->fam_blue->setViewStyle(0);
			SD->reg_red->setViewStyle(0);
			SD->reg_blue->setViewStyle(0);
			SD->fam_red->align = 0;
			SD->fam_blue->align = 0;
			SD->reg_red->align = 0;
			SD->reg_blue->align = 0;
			SD->sec->setFrameShape(QFrame::Box);
			SD->plus_red->setColor("white");
			SD->plus_blue->setColor("white");
			SD->repaint();
        //}
	}
	else if (ui.rbView2->isChecked()) {
		View = 1;
		ball_red->setViewStyle(1, ui.sbFrame->value());
		ball_blue->setViewStyle(2, ui.sbFrame->value());
		akt_red->setViewStyle(1, ui.sbFrame->value());
		akt_blue->setViewStyle(2, ui.sbFrame->value());
		fam_red->setViewStyle(1);
		fam_blue->setViewStyle(2);
		reg_red->setViewStyle(1);
		reg_blue->setViewStyle(2);
		fam_red->align = 0;
		fam_blue->align = 0;
		reg_red->align = 0;
		reg_blue->align = 0;
		sec->setFrameShape(QFrame::Box);
		plus_red->setColor("red");
		plus_blue->setColor("blue");
        //if (desk->numScreens() != 1) {
			SD->View = 1;
			SD->ball_red->setViewStyle(1, ui.sbFrame->value());
			SD->ball_blue->setViewStyle(2, ui.sbFrame->value());
			SD->akt_red->setViewStyle(1, ui.sbFrame->value());
			SD->akt_blue->setViewStyle(2, ui.sbFrame->value());
			SD->fam_red->setViewStyle(1);
			SD->fam_blue->setViewStyle(2);
			SD->reg_red->setViewStyle(1);
			SD->reg_blue->setViewStyle(2);
			SD->fam_red->align = 1;
			SD->fam_blue->align = 2;
			SD->reg_red->align = 1;
			SD->reg_blue->align = 2;
			SD->sec->setFrameShape(QFrame::Box);
			SD->plus_red->setColor("red");
			SD->plus_blue->setColor("blue");
			SD->repaint();
        //}
	}
	else {
		View = 1;
		ball_red->setViewStyle(3, ui.sbFrame->value());
		ball_blue->setViewStyle(4, ui.sbFrame->value());
		akt_red->setViewStyle(3, ui.sbFrame->value());
		akt_blue->setViewStyle(4, ui.sbFrame->value());
		fam_red->setViewStyle(1);
		fam_blue->setViewStyle(2);
		reg_red->setViewStyle(1);
		reg_blue->setViewStyle(2);
		fam_red->align = 1;
		fam_blue->align = 2;
		reg_red->align = 1;
		reg_blue->align = 2;
		sec->setFrameShape(QFrame::NoFrame);
		plus_red->setColor("red");
		plus_blue->setColor("blue");
        //if (desk->numScreens() != 1) {
			SD->View = 1;
			SD->ball_red->setViewStyle(3,  ui.sbFrame->value());
			SD->ball_blue->setViewStyle(4, ui.sbFrame->value());
			SD->akt_red->setViewStyle(3,   ui.sbFrame->value());
			SD->akt_blue->setViewStyle(4,  ui.sbFrame->value());
			SD->fam_red->setViewStyle(1);
			SD->fam_blue->setViewStyle(2);
			SD->reg_red->setViewStyle(1);
			SD->reg_blue->setViewStyle(2);
			SD->fam_red->align =  1;
			SD->fam_blue->align = 2;
			SD->reg_red->align =  1;
			SD->reg_blue->align = 2;
			SD->sec->setFrameShape(QFrame::NoFrame);
			SD->plus_red->setColor("red");
			SD->plus_blue->setColor("blue");
			SD->repaint();
        //}
	}
		repaint();
}

void FirstDisplay::setFrameWidth(int f){
	sec->setLineWidth(f);
    SD->sec->setLineWidth(f);
	if (View == 0) {

		ball_red->setViewStyle(0, f);
		ball_blue->setViewStyle(0, f);
		akt_red->setViewStyle(0, f);
		akt_blue->setViewStyle(0, f);
		fam_red->setViewStyle(0);
		fam_blue->setViewStyle(0);
		reg_red->setViewStyle(0);
		reg_blue->setViewStyle(0);
		sec->setFrameShape(QFrame::Box);

        SD->ball_red->setViewStyle(0, f);
        SD->ball_blue->setViewStyle(0, f);
        SD->akt_red->setViewStyle(0, f);
        SD->akt_blue->setViewStyle(0, f);
        SD->fam_red->setViewStyle(0);
        SD->fam_blue->setViewStyle(0);
        SD->reg_red->setViewStyle(0);
        SD->reg_blue->setViewStyle(0);
        SD->sec->setFrameShape(QFrame::Box);
	}
	else {
		ball_red->setViewStyle(1, f);
		ball_blue->setViewStyle(2, f);
		akt_red->setViewStyle(1, f);
		akt_blue->setViewStyle(2, f);
		fam_red->setViewStyle(1);
		fam_blue->setViewStyle(2);
		reg_red->setViewStyle(1);
		reg_blue->setViewStyle(2);
		sec->setFrameShape(QFrame::Box);

        SD->ball_red->setViewStyle(1, f);
        SD->ball_blue->setViewStyle(2, f);
        SD->akt_red->setViewStyle(1, f);
        SD->akt_blue->setViewStyle(2, f);
        SD->fam_red->setViewStyle(1);
        SD->fam_blue->setViewStyle(2);
        SD->reg_red->setViewStyle(1);
        SD->reg_blue->setViewStyle(2);
        SD->sec->setFrameShape(QFrame::Box);
		//sec->setLineWidth(5);
	}
	
	repaint();
}

void FirstDisplay::setSpace(int s){
	grid->setSpacing(s);
    SD->grid->setSpacing(s);
	repaint();
}

void FirstDisplay::setSec(int m) {
    qDebug()<<m;
	//grid->setMargin(m);
	//repaint();
    grid->removeWidget(sec);
    SD->grid->removeWidget(SD->sec);
    if(m == 3){
        grid->addWidget(sec, 24, 24, 13, 20);
        SD->grid->addWidget(SD->sec, 24, 24, 13, 20);
    }else if(m == 2){
        grid->addWidget(sec, 25, 24, 11, 20);
        SD->grid->addWidget(SD->sec, 25, 24, 11, 20);
    }else{
        grid->addWidget(sec, 26, 24, 9, 20);
        SD->grid->addWidget(SD->sec, 26, 24, 9, 20);
    }
}

void FirstDisplay::changeSize(int i) {
    qDebug()<<"changeSize";
	if (i == 0) {
		if (HEIGHT_FAMILY < 5) {
			HEIGHT_FAMILY += 1;
			grid->setRowMinimumHeight(0, minimum_height + HEIGHT_FAMILY * percent_height / 6);
			grid->setRowMinimumHeight(1, minimum_height + HEIGHT_FAMILY * percent_height / 6);
			grid->setRowMinimumHeight(2, minimum_height + HEIGHT_FAMILY * percent_height / 6);
			grid->setRowMinimumHeight(3, minimum_height + HEIGHT_FAMILY * percent_height / 6);
			grid->setRowMinimumHeight(4, minimum_height + HEIGHT_FAMILY * percent_height / 6);
			grid->setRowMinimumHeight(5, minimum_height + HEIGHT_FAMILY * percent_height / 6);
            //if (desk->numScreens() != 1) {
				SD->grid->setRowMinimumHeight(0, SD->minimum_height + HEIGHT_FAMILY * SD->percent_height / 6);
				SD->grid->setRowMinimumHeight(1, SD->minimum_height + HEIGHT_FAMILY * SD->percent_height / 6);
				SD->grid->setRowMinimumHeight(2, SD->minimum_height + HEIGHT_FAMILY * SD->percent_height / 6);
				SD->grid->setRowMinimumHeight(3, SD->minimum_height + HEIGHT_FAMILY * SD->percent_height / 6);
				SD->grid->setRowMinimumHeight(4, SD->minimum_height + HEIGHT_FAMILY * SD->percent_height / 6);
				SD->grid->setRowMinimumHeight(5, SD->minimum_height + HEIGHT_FAMILY * SD->percent_height / 6);

            //}
			if (HEIGHT_FAMILY == -5) {
				fam_red->setVisible(true);
				fam_blue->setVisible(true);
				grid->addWidget(fam_red, 0, 0, 6, 34);
				grid->addWidget(fam_blue, 0, 34, 6, 34);
                //if (desk->numScreens() != 1) {
					SD->fam_red->setVisible(true);
					SD->fam_blue->setVisible(true);
					SD->grid->addWidget(SD->fam_red, 0, 34, 6, 34);
					SD->grid->addWidget(SD->fam_blue, 0, 0, 6, 34);
                //}
			}
		}
	}
	else if (i == 1) {
		if (HEIGHT_FAMILY > -6) {
			HEIGHT_FAMILY -= 1;
			if (HEIGHT_FAMILY != -6) {
				grid->setRowMinimumHeight(0, minimum_height + HEIGHT_FAMILY * percent_height / 6);
				grid->setRowMinimumHeight(1, minimum_height + HEIGHT_FAMILY * percent_height / 6);
				grid->setRowMinimumHeight(2, minimum_height + HEIGHT_FAMILY * percent_height / 6);
				grid->setRowMinimumHeight(3, minimum_height + HEIGHT_FAMILY * percent_height / 6);
				grid->setRowMinimumHeight(4, minimum_height + HEIGHT_FAMILY * percent_height / 6);
				grid->setRowMinimumHeight(5, minimum_height + HEIGHT_FAMILY * percent_height / 6);
                //if (desk->numScreens() != 1) {
					SD->grid->setRowMinimumHeight(0, SD->minimum_height + HEIGHT_FAMILY * SD->percent_height / 6);
					SD->grid->setRowMinimumHeight(1, SD->minimum_height + HEIGHT_FAMILY * SD->percent_height / 6);
					SD->grid->setRowMinimumHeight(2, SD->minimum_height + HEIGHT_FAMILY * SD->percent_height / 6);
					SD->grid->setRowMinimumHeight(3, SD->minimum_height + HEIGHT_FAMILY * SD->percent_height / 6);
					SD->grid->setRowMinimumHeight(4, SD->minimum_height + HEIGHT_FAMILY * SD->percent_height / 6);
					SD->grid->setRowMinimumHeight(5, SD->minimum_height + HEIGHT_FAMILY * SD->percent_height / 6);
                //}
			}
			else {
				grid->setRowMinimumHeight(0, 0);
				grid->setRowMinimumHeight(1, 0);
				grid->setRowMinimumHeight(2, 0);
				grid->setRowMinimumHeight(3, 0);
				grid->setRowMinimumHeight(4, 0);
				grid->setRowMinimumHeight(5, 0);
				fam_red->setVisible(false);
				fam_blue->setVisible(false);
				grid->removeWidget(fam_red);
				grid->removeWidget(fam_blue);
                //if (desk->numScreens() != 1) {
					SD->grid->setRowMinimumHeight(0, 0);
					SD->grid->setRowMinimumHeight(1, 0);
					SD->grid->setRowMinimumHeight(2, 0);
					SD->grid->setRowMinimumHeight(3, 0);
					SD->grid->setRowMinimumHeight(4, 0);
					SD->grid->setRowMinimumHeight(5, 0);
					SD->fam_red->setVisible(false);
					SD->fam_blue->setVisible(false);
					SD->grid->removeWidget(SD->fam_red);
					SD->grid->removeWidget(SD->fam_blue);

                //}
			}

		}
	}
	else if (i == 2) {
		if (HEIGHT_REGION < 5) {
			HEIGHT_REGION += 1;
			grid->setRowMinimumHeight(37, minimum_height + HEIGHT_REGION * percent_height / 5);
			grid->setRowMinimumHeight(38, minimum_height + HEIGHT_REGION * percent_height / 5);
			grid->setRowMinimumHeight(39, minimum_height + HEIGHT_REGION * percent_height / 5);
			grid->setRowMinimumHeight(40, minimum_height + HEIGHT_REGION * percent_height / 5);
			grid->setRowMinimumHeight(41, minimum_height + HEIGHT_REGION * percent_height / 5);
            //if (desk->numScreens() != 1) {
				SD->grid->setRowMinimumHeight(37, SD->minimum_height + HEIGHT_REGION * SD->percent_height / 5);
				SD->grid->setRowMinimumHeight(38, SD->minimum_height + HEIGHT_REGION * SD->percent_height / 5);
				SD->grid->setRowMinimumHeight(39, SD->minimum_height + HEIGHT_REGION * SD->percent_height / 5);
				SD->grid->setRowMinimumHeight(40, SD->minimum_height + HEIGHT_REGION * SD->percent_height / 5);
				SD->grid->setRowMinimumHeight(41, SD->minimum_height + HEIGHT_REGION * SD->percent_height / 5);
            //}
			if (HEIGHT_REGION == -5) {
				reg_red->setVisible(true);
				reg_blue->setVisible(true);
				grid->addWidget(reg_red, 37, 0, 5, 34);
				grid->addWidget(reg_blue, 37, 34, 5, 34);

				flag_red->setVisible(true);
				flag_blue->setVisible(true);
				grid->addWidget(flag_red, 29, 14, 8, 10);
				grid->addWidget(flag_blue, 29, 44, 8, 10);

				grid->removeWidget(np_red);
				grid->removeWidget(np_blue);
				grid->addWidget(np_red, 24, 19, 5, 5);
				grid->addWidget(np_blue, 24, 44, 5, 5);

				grid->removeWidget(nv_red);
				grid->removeWidget(nv_blue);
				grid->addWidget(nv_red, 24, 14, 5, 5);
				grid->addWidget(nv_blue, 24, 49, 5, 5);
                //if (desk->numScreens() != 1) {
					SD->reg_blue->setVisible(true);
					SD->reg_red->setVisible(true);
					SD->grid->addWidget(SD->reg_red, 37, 34, 5, 34);
					SD->grid->addWidget(SD->reg_blue, 37, 0, 5, 34);

					SD->flag_red->setVisible(true);
					SD->flag_blue->setVisible(true);
					SD->grid->addWidget(SD->flag_red, 29, 14, 8, 10);
					SD->grid->addWidget(SD->flag_blue, 29, 44, 8, 10);

					SD->grid->removeWidget(SD->np_blue);
					SD->grid->removeWidget(SD->np_red);
					SD->grid->addWidget(SD->np_red, 24, 44, 5, 5);
					SD->grid->addWidget(SD->np_blue, 24, 19, 5, 5);

					SD->grid->removeWidget(SD->nv_blue);
					SD->grid->removeWidget(SD->nv_red);
					SD->grid->addWidget(SD->nv_red, 24, 49, 5, 5);
					SD->grid->addWidget(SD->nv_blue, 24, 14, 5, 5);
                //}
			}
		}
	}
	else {
		if (HEIGHT_REGION > -6) {
			HEIGHT_REGION -= 1;
			if (HEIGHT_REGION != -6) {
				grid->setRowMinimumHeight(37, minimum_height + HEIGHT_REGION * percent_height / 5);
				grid->setRowMinimumHeight(38, minimum_height + HEIGHT_REGION * percent_height / 5);
				grid->setRowMinimumHeight(39, minimum_height + HEIGHT_REGION * percent_height / 5);
				grid->setRowMinimumHeight(40, minimum_height + HEIGHT_REGION * percent_height / 5);
				grid->setRowMinimumHeight(41, minimum_height + HEIGHT_REGION * percent_height / 5);
                //if (desk->numScreens() != 1) {
					SD->grid->setRowMinimumHeight(37, SD->minimum_height + HEIGHT_REGION * SD->percent_height / 5);
					SD->grid->setRowMinimumHeight(38, SD->minimum_height + HEIGHT_REGION * SD->percent_height / 5);
					SD->grid->setRowMinimumHeight(39, SD->minimum_height + HEIGHT_REGION * SD->percent_height / 5);
					SD->grid->setRowMinimumHeight(40, SD->minimum_height + HEIGHT_REGION * SD->percent_height / 5);
					SD->grid->setRowMinimumHeight(41, SD->minimum_height + HEIGHT_REGION * SD->percent_height / 5);
                //}
			}
			else {
				grid->setRowMinimumHeight(37, 0);
				grid->setRowMinimumHeight(38, 0);
				grid->setRowMinimumHeight(39, 0);
				grid->setRowMinimumHeight(40, 0);
				grid->setRowMinimumHeight(41, 0);
				reg_red->setVisible(false);
				reg_blue->setVisible(false);
				grid->removeWidget(reg_red);
				grid->removeWidget(reg_blue);

				grid->removeWidget(flag_red);
				grid->removeWidget(flag_blue);
				flag_red->setVisible(false);
				flag_blue->setVisible(false);

				grid->removeWidget(np_blue);
				grid->removeWidget(np_red);
				grid->addWidget(np_red, 28, 19, 5, 5);
				grid->addWidget(np_blue, 28, 44, 5, 5);

				grid->removeWidget(nv_blue);
				grid->removeWidget(nv_red);
				grid->addWidget(nv_red, 28, 14, 5, 5);
				grid->addWidget(nv_blue, 28, 49, 5, 5);
                //if (desk->numScreens() != 1) {
					SD->grid->setRowMinimumHeight(37, 0);
					SD->grid->setRowMinimumHeight(38, 0);
					SD->grid->setRowMinimumHeight(39, 0);
					SD->grid->setRowMinimumHeight(40, 0);
					SD->grid->setRowMinimumHeight(41, 0);
					SD->reg_blue->setVisible(false);
					SD->reg_red->setVisible(false);
					SD->grid->removeWidget(SD->reg_blue);
					SD->grid->removeWidget(SD->reg_red);

					SD->grid->removeWidget(SD->flag_red);
					SD->grid->removeWidget(SD->flag_blue);
					SD->flag_red->setVisible(false);
					SD->flag_blue->setVisible(false);

					SD->grid->removeWidget(SD->np_blue);
					SD->grid->removeWidget(SD->np_red);
					SD->grid->addWidget(SD->np_red, 28, 44, 5, 5);
					SD->grid->addWidget(SD->np_blue, 28, 19, 5, 5);

					SD->grid->removeWidget(SD->nv_red);
					SD->grid->removeWidget(SD->nv_blue);
					SD->grid->addWidget(SD->nv_red, 28, 49, 5, 5);
					SD->grid->addWidget(SD->nv_blue, 28, 14, 5, 5);
                //}
			}
		}
	}
    //SD->grid->removeWidget(SD->rV);
    //SD->grid->addWidget(SD->rV, 0, 0, 42, 68);
    //SD->repaint();
    //SD->rV->raise();
    SD->rV->setGeometry(0, 0, SD->width(), SD->height());
}

void FirstDisplay::test(){
    qDebug()<<"test";
}

#include <QPainter>
#include <QGridLayout>
#include <QPushButton>
#include <QEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>

#include <QMessageBox>
#include <QPalette>
#include <QProcess>
#include <QFileDialog>
#include <QTime>
#include "pcscreen.h"

#include <math.h>


#include "list_family.h"
#include "category.h"
#include "lcdstopwatch.h"

//#include <QHostAddress>
//#include <QNetworkInterface>

WidgetFilter::WidgetFilter(QObject* pobj) : QObject(pobj){
    qDebug()<<"constructor event";
}

WidgetFilter::~WidgetFilter(){
    qDebug()<<"destructor event";
}

bool WidgetFilter::eventFilter(QObject* , QEvent* pe){
    if(pe->type() == QEvent::Close){
        sigClose();
        return true;
    }
    return false;
}

/*
class MyEvent : public QEvent {
public:
	MyEvent(int ev) : QEvent{ (Type)(QEvent::User + ev) } {}

};
*/

PCScreen::PCScreen(QWidget * parent) : QWidget(parent){
    //ev_L = new MyEvent(200);
    //ev_R = new MyEvent(201);

	col_red = "white";
	col_blue = "white";

	HEIGHT_REGION = 0;
	HEIGHT_FAMILY = 0;
	minimum_height = 0;
	percent_height = 0;

    View = 0;			//красно-синий фон

    rateRed = new Rate(this);
    rateRed->setFrameShape(QFrame::Box);
    rateRed->setObjectName("ball_red");

    rateBlue = new Rate(this);
    rateBlue->setFrameShape(QFrame::Box);
    rateBlue->setObjectName("ball_blue");

    actRed = new Rate(this);
    actRed->setFrameShape(QFrame::Box);
    actRed->setObjectName("akt_red");

    actBlue = new Rate(this);
    actBlue->setFrameShape(QFrame::Box);
    actBlue->setObjectName("akt_blue");

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
    //btnParter_red->setFocusPolicy(Qt::NoFocus);
	
	QPushButton * btnTime = new QPushButton(u8"ВРЕМЯ", this);
    btnTime->setObjectName("btnTime");
	btnTime->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	//btnTime->setStyleSheet("color: green; font: bold " + QString::number(round(btnTime->height() / 2)) + "px;");
    //btnTime->setFocusPolicy(Qt::NoFocus);

	QPushButton * btnParter_blue = new QPushButton(u8"ПАРТЕР", this);
	btnParter_blue->setObjectName("btnParter_blue");
	btnParter_blue->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	btnParter_blue->setStyleSheet("color: blue; font: bold " + QString::number(round(btnParter_blue->height() / 2)) + "px;");
    //btnParter_blue->setFocusPolicy(Qt::NoFocus);
	
	QPushButton * btnTehTime_red = new QPushButton(u8"Т.ВРЕМЯ", this);
	btnTehTime_red->setObjectName("btnTehTime_red");
	btnTehTime_red->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	btnTehTime_red->setStyleSheet("color: red; font: bold " + QString::number(round(btnTehTime_red->height() / 2)) + "px;");
    //btnTehTime_red->setFocusPolicy(Qt::NoFocus);

    QPushButton * btnSettings = new QPushButton("СПОРТСМЕНЫ", this);
	btnSettings->setObjectName("btnSettings");
	btnSettings->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	btnSettings->setStyleSheet("color: green; font: bold " + QString::number(round(btnSettings->height() / 2)) + "px;");
    //btnSettings->setFocusPolicy(Qt::NoFocus);
	
	QPushButton * btnTehTime_blue = new QPushButton(u8"Т.ВРЕМЯ", this);
	btnTehTime_blue->setObjectName("btnTehTime_blue");
	btnTehTime_blue->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	btnTehTime_blue->setStyleSheet("color: blue; font: bold " + QString::number(round(btnTehTime_blue->height() / 2)) + "px;");
    //btnTehTime_blue->setFocusPolicy(Qt::NoFocus);

	QPushButton * btnPlus_red = new QPushButton("+", this);
	btnPlus_red->setObjectName("btnPlus_red");
	btnPlus_red->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	btnPlus_red->setStyleSheet("color: red; font: bold " + QString::number(btnPlus_red->height()) + "px;");
    //btnPlus_red->setFocusPolicy(Qt::NoFocus);

	QPushButton * btnPlus_blue = new QPushButton("+", this);
	btnPlus_blue->setObjectName("btnPlus_blue");
	btnPlus_blue->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	btnPlus_blue->setStyleSheet("color: blue; font: bold " + QString::number(btnPlus_blue->height()) + "px;");
    //btnPlus_blue->setFocusPolicy(Qt::NoFocus);

    QPushButton * btnView = new QPushButton("НАСТРОЙКИ", this);
	//btnView->setObjectName("btnPlus_blue");
	btnView->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	btnView->setStyleSheet("font: bold " + QString::number(round(btnView->height() / 2)) + "px;");
    //btnView->setFocusPolicy(Qt::NoFocus);

    plus_red = new Plus(col_red, this);
	plus_red->setObjectName("plus_red");
    plus_blue = new Plus(col_blue, this);
	plus_blue->setObjectName("plus_blue");

    mainTimer = new LCDTimer(this);
    mainTimer->setObjectName("mainTimer");
    //connect(btnTime, SIGNAL(clicked()), mainTimer, SLOT(StartStop()));

    LCDStopwatch * sec_red = new LCDStopwatch(this, "0:20", QColor(255, 0, 0), QColor(255, 102, 102), true, true);
	sec_red->setObjectName("sec_red");
	sec_red->hide();

    LCDStopwatch * sec_blue = new LCDStopwatch(this, "0:20", QColor(0, 0, 255), QColor(102, 102, 255), true, true);
	sec_blue->setObjectName("sec_blue");
	sec_blue->hide();

    LCDStopwatch * sec_red_t = new LCDStopwatch(this, "2:00", QColor(255, 0, 0), QColor(255, 102, 102), true, true);
    sec_red_t->setObjectName("sec_red_t");
    sec_red_t->hide();

    LCDStopwatch * sec_blue_t = new LCDStopwatch(this, "2:00", QColor(0, 0, 255), QColor(102, 102, 255), true, true);
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

    cat = new QPushButton(this);//("yellow", this);
    cat->setAutoFillBackground(true);
    cat->setStyleSheet("background-color: black; color: white; text-align: center");


    ListFamily * lf = new ListFamily(this);
	lf->setObjectName("lf");
    connect(lf->weight, SIGNAL(activated(const QString&)), this, SLOT(setCat(QString)));

	formView = new QWidget;
    ui.setupUi(formView);
    WidgetFilter* wf = new WidgetFilter(formView);
    formView->installEventFilter(wf);
    connect(wf, SIGNAL(sigClose()), this, SLOT(closeView()));


    //connect(formView, SIGNAL())

    connect(ui.btnNameDown, SIGNAL(clicked()), this, SLOT(changeSize()));
    connect(ui.btnNameUp,   SIGNAL(clicked()), this, SLOT(changeSize()));
    connect(ui.btnRegDown,  SIGNAL(clicked()), this, SLOT(changeSize()));
    connect(ui.btnRegUp,    SIGNAL(clicked()), this, SLOT(changeSize()));

    frmTime = new QWidget;
    uiTime.setupUi(frmTime);

    connect(btnView, SIGNAL(clicked(bool)), this, SLOT(showView()));

	connect(ui.rbView1, SIGNAL(toggled(bool)), this, SLOT(setView(void)));
	connect(ui.rbView2, SIGNAL(toggled(bool)), this, SLOT(setView(void)));
	connect(ui.rbView3, SIGNAL(toggled(bool)), this, SLOT(setView(void)));
	connect(ui.sbFrame, SIGNAL(valueChanged(int)), this, SLOT(setFrameWidth(int)));
	connect(ui.sbSpacing, SIGNAL(valueChanged(int)), this, SLOT(setSpace(int)));
	connect(ui.sbSec, SIGNAL(valueChanged(int)), this, SLOT(setSec(int)));
	
	flag_blue = new QLabel(this);
	flag_blue->setScaledContents(true);
	//flag_blue->setAutoFillBackground(true);

	flag_red = new QLabel(this);
	flag_red->setScaledContents(true);
	//flag_red->setAutoFillBackground(true);

    viewCam1 = new CameraViewer;
    viewCam1->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    viewCam2 = new CameraViewer;
    viewCam2->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    btnPlayLastWithSound1 = new QPushButton("Просмотр\nсо звуком");
    btnPlayLastWithSound1->setObjectName("btnPlayLastWithSound1");
    btnPlayLastWithSound1->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    btnPlayLastWithSound1->setStyleSheet("font-size: 10pt");
    connect(btnPlayLastWithSound1, SIGNAL(clicked()), SLOT(PlayFile()));

    btnPlayLastWithSound2 = new QPushButton("Просмотр\nсо звуком");
    btnPlayLastWithSound2->setObjectName("btnPlayLastWithSound2");
    btnPlayLastWithSound2->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    btnPlayLastWithSound1->setStyleSheet("font-size: 10pt");
    connect(btnPlayLastWithSound2, SIGNAL(clicked()), SLOT(PlayFile()));

    btnPlayLastSlowMotion1 = new QPushButton("Просмотр\nзамедленный");
    btnPlayLastSlowMotion1->setObjectName("btnPlayLastSlowMotion1");
    btnPlayLastSlowMotion1->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    btnPlayLastSlowMotion1->setStyleSheet("font-size: 10pt");
    connect(btnPlayLastSlowMotion1, SIGNAL(clicked()), SLOT(PlaySlowMotion()));

    btnPlayLastSlowMotion2 = new QPushButton("Просмотр\nзамедленный");
    btnPlayLastSlowMotion2->setObjectName("btnPlayLastSlowMotion2");
    btnPlayLastSlowMotion2->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    btnPlayLastSlowMotion2->setStyleSheet("font-size: 10pt");
    connect(btnPlayLastSlowMotion2, SIGNAL(clicked()), SLOT(PlaySlowMotion()));

    btnPlaySlowMotion = new QPushButton("Просмотр с выбором файла");
    btnPlaySlowMotion->setObjectName("btnPlaySlowMotion");
    btnPlaySlowMotion->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    btnPlaySlowMotion->setStyleSheet("font-size: 10pt");
    connect(btnPlaySlowMotion, SIGNAL(clicked()), SLOT(PlaySelectedFile()));

    btnStopRecord = new QPushButton("Стоп запись");
    btnStopRecord->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    btnStopRecord->setStyleSheet("font-size: 10pt");
    connect(btnStopRecord, SIGNAL(clicked()), SLOT(StopRecord()));
    //connect(mainTimer, SIGNAL(sigStopped(bool)), btnStopRecord, SLOT(setEnabled(bool)));

    cbCam1 = new QCheckBox("Включить камеру 1");
    cbCam1->setObjectName("cbCam1");
    cbCam1->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    connect(cbCam1, SIGNAL(toggled(bool)), this, SLOT(turnCamera(bool)));

    cbCam2 = new QCheckBox("Включить камеру 2");
    cbCam2->setObjectName("cbCam2");
    cbCam2->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    connect(cbCam2, SIGNAL(toggled(bool)), this, SLOT(turnCamera(bool)));

    connect(ui.cbAutoCam1, SIGNAL(toggled(bool)), this, SLOT(autoCamera(bool)));
    connect(ui.cbAutoCam2, SIGNAL(toggled(bool)), this, SLOT(autoCamera(bool)));

    QLabel* lbl = new QLabel("Последний записанный файл");
    lbl->setAlignment(Qt::AlignCenter);
    lbl->setStyleSheet("color: white; border-style: solid;"
                                     "border-width: 2px;"
                                     "border-color: white;"
                                     "border-top-style: none;"
                                     "border-bottom-left-radius: 10px;"
                                     "border-bottom-right-radius: 10px;"
                                     "font-size: 12pt");
/*
    QString ip = "";
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    for(int nIter=0; nIter<list.count(); nIter++){
        if(!list[nIter].isLoopback())
            if (list[nIter].protocol() == QAbstractSocket::IPv4Protocol ){
                ip = list[nIter].toString();
                break;
            }
    }
*/
    QLabel* lbl2 = new QLabel("Установка времени боя - 'F1', сброс - 'Backspace', выход - 'Esc'");
    lbl2->setAlignment(Qt::AlignCenter);
    lbl2->setStyleSheet("color: white; font-size: 12pt");

    lblCpuUsage = new QLabel;
    lblCpuUsage->setAlignment(Qt::AlignCenter);
    lblCpuUsage->setStyleSheet("background-color: black; color: white; font-size: 12pt");

    //lblTv.hide();

	grid = new QGridLayout(this);
	grid->setObjectName("grid");
	//spacing = 6;
	//margin = 6;
    //сетка 68х42
	grid->setSpacing(6);
	grid->setMargin(6);
    grid->addWidget(fam_red,                0,  0,  4,  34);
    grid->addWidget(fam_blue,               0,  34, 4,  34);

    grid->addWidget(rateRed,                4,  0,  20, 24);
    grid->addWidget(rateBlue,               4,  44, 20, 24);
    grid->addWidget(actRed,                 24, 0,  14, 14);
    grid->addWidget(actBlue,                24, 54, 14, 14);

    grid->addWidget(np_red,                 24, 19, 5,  5);
    grid->addWidget(np_blue,                24, 44, 5,  5);

    grid->addWidget(nv_red,                 24, 14, 5,  5);
    grid->addWidget(nv_blue,                24, 49, 5,  5);

    grid->addWidget(btnTehTime_red,         4,  24, 2,  6);
    grid->addWidget(btnSettings,            4,  30, 2,  8);
    grid->addWidget(btnTehTime_blue,        4,  38, 2,  6);

    grid->addWidget(btnPlus_red,            6,  30, 2,  3);
    grid->addWidget(lblCpuUsage,            6,  33, 2,  2);
    grid->addWidget(btnPlus_blue,           6,  35, 2,  3);

    grid->addWidget(plus_red,               4,  0, 4,  4);
    grid->addWidget(plus_blue,              4,  44, 4,  4);

    grid->addWidget(cat,                    6, 38, 2,  6);

    grid->addWidget(btnView,                6, 24, 2,  6);

    grid->addWidget(lbl2,                   8, 24, 2,  20);

    grid->addWidget(btnParter_red,          22, 24, 2,  6);
    grid->addWidget(btnTime,                22, 31, 2,  6);
    grid->addWidget(btnParter_blue,         22, 38, 2,  6);


    //grid->addWidget(btnStartPlay,         20, 38, 2,  3);

    grid->addWidget(mainTimer,              24, 24, 14, 20);

    grid->addWidget(sec_red,                9,  0,  12, 24);
    grid->addWidget(sec_blue,               9,  44, 12, 24);

    grid->addWidget(sec_red_t,              9,  0,  12, 24);
    grid->addWidget(sec_blue_t,             9,  44, 12, 24);

    grid->addWidget(flag_red,               29, 14, 8,  10);
    grid->addWidget(flag_blue,              29, 44, 8,  10);
	
    grid->addWidget(reg_red,                38, 0,  4,  34);
    grid->addWidget(reg_blue,               38, 34, 4,  34);

    grid->addWidget(viewCam1,               10, 24,  6, 10);
    grid->addWidget(btnPlayLastWithSound1,  16, 24,  2,  5);
    grid->addWidget(btnPlayLastSlowMotion1, 16, 29,  2,  5);
    grid->addWidget(btnPlaySlowMotion,      20, 24,  2, 10);
    grid->addWidget(cbCam1,                 15, 25,  1,  8);
    grid->addWidget(viewCam2,               10, 34,  6, 10);
    grid->addWidget(btnPlayLastWithSound2,  16, 34,  2,  5);
    grid->addWidget(btnPlayLastSlowMotion2, 16, 39,  2,  5);
    grid->addWidget(btnStopRecord,          20, 34,  2, 10);
    grid->addWidget(cbCam2,                 15 , 35,  1, 8);

    grid->addWidget(lbl,                    18, 24,  2, 20);

    //grid->addWidget(&lblTv,                 21,  0, 21, 34);

    //grid->addWidget(rV,              0, 13,  23,  42);

    //connect(rV, SIGNAL(end_replay()), SLOT(removeReplay()));
    //rV->hide();
    //minimum_height_family = (height() - 12) / 42;
	//minimum_height_region = minimum_height_family;

    btnTime->setStyleSheet("color: green; font: bold " + QString::number(round(btnTime->height() / 2)) + "px;");

    //desk = QApplication::desktop();

    //if (desk->numScreens() == 2){
    if(QGuiApplication::screens().count() == 2){
        showFullScreen();
    }else{      
        show();
        setGeometry(QApplication::desktop()->availableGeometry(this).width() / 2, QApplication::desktop()->availableGeometry(this).height() / 2,
                    QApplication::desktop()->availableGeometry(this).width() / 2, QApplication::desktop()->availableGeometry(this).height() / 2);
    }

    //if (desk->numScreens() == 1) {
    if(QGuiApplication::screens().count() == 1){
        QMessageBox::information(this, "ВНИМАНИЕ!",
        "Подключите к компьютеру дисплей в режиме \"Расширенный рабочий стол!\"",
		QMessageBox::Ok);
	}
    //else {
        tvScreen = new TVScreen();

    //if (desk->numScreens() == 1)
    if(QGuiApplication::screens().count() == 1)
        tvScreen->setGeometry(0, 0, QApplication::desktop()->availableGeometry(this).width() / 2, QApplication::desktop()->availableGeometry(this).height() / 2);
    else{
        tvScreen->setGeometry(width(), 0, 100, height());
        tvScreen->setGeometry(QApplication::desktop()->availableGeometry(this).right(),
                        0, QApplication::desktop()->availableGeometry(tvScreen).width(),
                        QApplication::desktop()->availableGeometry(tvScreen).height());
    }

        tvScreen->show();

        connect(rateRed,	SIGNAL(sigRate(int)),		tvScreen->ball_red,	SLOT(setRate(int)));
        connect(rateBlue,	SIGNAL(sigRate(int)),		tvScreen->ball_blue,SLOT(setRate(int)));
        connect(actRed,     SIGNAL(sigRate(int)),		tvScreen->akt_red,	SLOT(setRate(int)));
        connect(actBlue,	SIGNAL(sigRate(int)),		tvScreen->akt_blue,	SLOT(setRate(int)));

        connect(np_red,		SIGNAL(ball(QString)),	tvScreen->np_red,		SLOT(setValue(QString)));
        connect(np_blue,	SIGNAL(ball(QString)),	tvScreen->np_blue,	SLOT(setValue(QString)));
        connect(nv_red,		SIGNAL(ball(QString)),	tvScreen->nv_red,		SLOT(setValue(QString)));
        connect(nv_blue,	SIGNAL(ball(QString)),	tvScreen->nv_blue,	SLOT(setValue(QString)));

        connect(plus_red,   SIGNAL(textChange(QString)), tvScreen->plus_red, SLOT(setData(QString)));
        connect(plus_blue,  SIGNAL(textChange(QString)), tvScreen->plus_blue, SLOT(setData(QString)));

        connect(fam_red,    SIGNAL(sigText(QString)), tvScreen->fam_red, SLOT(Text(QString)));
        connect(fam_blue,   SIGNAL(sigText(QString)), tvScreen->fam_blue, SLOT(Text(QString)));
        connect(reg_red,    SIGNAL(sigText(QString)), tvScreen->reg_red, SLOT(Text(QString)));
        connect(reg_blue,   SIGNAL(sigText(QString)), tvScreen->reg_blue, SLOT(Text(QString)));

        //connect(cat,  SIGNAL(sigText(QString)), tvScreen->cat, SLOT(setText(QString)));

        connect(mainTimer, SIGNAL(sigTime(QString, QPalette)), tvScreen->sec, SLOT(showTime(QString, QPalette)));
        //connect(btnSettings, SIGNAL(clicked()), this, SIGNAL(setCategory()));

        connect(uiTime.dMin, SIGNAL(valueChanged(int)), this, SLOT(setTime()));
        connect(uiTime.dSec, SIGNAL(valueChanged(int)), this, SLOT(setTime()));
        connect(uiTime.dSec2, SIGNAL(valueChanged(int)), this, SLOT(setTime()));


    //}

    connect(sec_red, SIGNAL(sigVisible(bool)), tvScreen->sec_red, SLOT(setVisible(bool)));
    connect(sec_red,  SIGNAL(sigTime(QString, QPalette)), tvScreen->sec_red, SLOT(showTime(QString, QPalette)));

    connect(sec_blue, SIGNAL(sigVisible(bool)), tvScreen->sec_blue, SLOT(setVisible(bool)));
    connect(sec_blue,  SIGNAL(sigTime(QString, QPalette)), tvScreen->sec_blue, SLOT(showTime(QString, QPalette)));

    connect(sec_red_t, SIGNAL(sigVisible(bool)), tvScreen->sec_red_t, SLOT(setVisible(bool)));
    connect(sec_red_t,  SIGNAL(sigTime(QString, QPalette)), tvScreen->sec_red_t, SLOT(showTime(QString, QPalette)));

    connect(sec_blue_t, SIGNAL(sigVisible(bool)), tvScreen->sec_blue_t, SLOT(setVisible(bool)));
    connect(sec_blue_t,  SIGNAL(sigTime(QString, QPalette)), tvScreen->sec_blue_t, SLOT(showTime(QString, QPalette)));


    ui.leCam1->setObjectName("leCam1");
    ui.leCam2->setObjectName("leCam2");
    connect(ui.leCam1, SIGNAL(editingFinished()), this, SLOT(setCam()));
    connect(ui.leCam2, SIGNAL(editingFinished()), this, SLOT(setCam()));

    QFile cam1("cam1.txt");
    if(!cam1.exists()){
        cam1.open(QIODevice::WriteOnly);
        cam1.close();
    }
    else{
        cam1.open(QIODevice::ReadOnly);
        cam1Url = cam1.readAll();
        cam1.close();
    }

    QFile cam2("cam2.txt");
    if(!cam2.exists()){
        cam2.open(QIODevice::WriteOnly);
        cam2.close();
    }
    else{
        cam2.open(QIODevice::ReadOnly);
        cam2Url = cam2.readAll();
        cam2.close();
    }

    camera1 = new Camera;
    camera1->setObjectName("camera1");
    threadCam1 = new QThread;
    camera1->moveToThread(threadCam1);
    connect(threadCam1, SIGNAL(started()),        camera1,  SLOT(TurnOnCamera()));
    connect(camera1,    SIGNAL(sigImage(QImage)), viewCam1, SLOT(draw_image(QImage)));
    connect(camera1,    SIGNAL(finished()),       this,     SLOT(finishedCamera()));
    connect(ui.cbKey,   SIGNAL(toggled(bool)),    camera1,  SLOT(onlyKeyFrame(bool)), Qt::DirectConnection);

    camera2 = new Camera;
    camera2->setObjectName("camera2");
    threadCam2 = new QThread;
    camera2->moveToThread(threadCam2);
    connect(threadCam2, SIGNAL(started()),        camera2,  SLOT(TurnOnCamera()));
    connect(camera2,    SIGNAL(sigImage(QImage)), viewCam2, SLOT(draw_image(QImage)));
    connect(camera2,    SIGNAL(finished()),       this,     SLOT(finishedCamera()));
    connect(ui.cbKey,   SIGNAL(toggled(bool)),    camera2,  SLOT(onlyKeyFrame(bool)), Qt::DirectConnection);

    connect(mainTimer, SIGNAL(sigStarted(bool)), this, SLOT(StartRecord(bool)));
    connect(mainTimer, SIGNAL(sigReset()), this, SLOT(StopRecord()));

    QDir dir("video");
    if(!dir.exists()){
        QDir dir;
        dir.mkdir("video");
    }

    QTimer* tmr = new QTimer(this);
    connect(tmr, SIGNAL(timeout()), this, SLOT(drawTvScreenshot()));
    tmr->start(100);

    QTimer* tmrCpu = new QTimer(this);
    connect(tmrCpu, SIGNAL(timeout()), this, SLOT(CpuUsage()));
    tmrCpu->start(1000);

}

void PCScreen::CpuUsage(){
    static ULARGE_INTEGER TimeIdle, TimeKernel, TimeUser;
    FILETIME Idle, Kernel, User;
    ULARGE_INTEGER uIdle, uKernel, uUser;
    GetSystemTimes(&Idle, &Kernel, &User);
    memcpy(&uIdle, &Idle, sizeof(FILETIME));
    memcpy(&uKernel, &Kernel, sizeof(FILETIME));
    memcpy(&uUser, &User, sizeof(FILETIME));
    long long t;
    t = (((((uKernel.QuadPart-TimeKernel.QuadPart)+(uUser.QuadPart-TimeUser.QuadPart))-
        (uIdle.QuadPart-TimeIdle.QuadPart))*(100))/((uKernel.QuadPart-
            TimeKernel.QuadPart)+(uUser.QuadPart-TimeUser.QuadPart)));
    TimeIdle.QuadPart = uIdle.QuadPart;
    TimeUser.QuadPart = uUser.QuadPart;
    TimeKernel.QuadPart = uKernel.QuadPart;
    lblCpuUsage->setText(QString::number(t) + " %");
    //qDebug()<<"cpu usage = "<<t<<"%";
}

void PCScreen::setCat(QString s){
    cat->setText(s);
    tvScreen->cat->setText(s);
}

void PCScreen::autoCamera(bool state){
    if(state){
        if(sender()->objectName() == "cbAutoCam1"){
            ui.cbAutoCam2->setEnabled(false);
            camConn = new CameraConnection(this);
            ui.leCam1->setText("");
            ui.leCam1->setStyleSheet("background-color: red");
        }
        else{
            ui.cbAutoCam1->setEnabled(false);
            camConn = new CameraConnection(this, 2);
            ui.leCam2->setText("");
            ui.leCam2->setStyleSheet("background-color: red");
        }
        connect(camConn, SIGNAL(sigCamera(QString)), this, SLOT(setCamera(QString)));
    }
    else{
        if(sender()->objectName() == "cbAutoCam1"){
            ui.cbAutoCam2->setEnabled(true);
            ui.leCam1->setStyleSheet("background-color: white");
            QFile f("cam1.txt");
            f.open(QIODevice::ReadOnly);
            ui.leCam1->setText(f.readLine());
            f.close();
        }
        else{
            ui.cbAutoCam1->setEnabled(true);
            ui.leCam2->setStyleSheet("background-color: white");
            QFile f("cam2.txt");
            f.open(QIODevice::ReadOnly);
            ui.leCam2->setText(f.readLine());
            f.close();
        }
        if(camConn){
            disconnect(camConn, SIGNAL(sigCamera(QString)), nullptr, nullptr);
            camConn->deleteLater();
        }
    }
}

void PCScreen::setCamera(QString ip){
    if(ui.cbAutoCam1->isChecked()){
        QFile f;
        QTextStream out(&f);
        f.setFileName("cam1.txt");
        f.open(QIODevice::WriteOnly);
        out << "srt://" + ip + ":1111";
        f.close();
        cam1Url = "srt://" + ip + ":1111";
        ui.cbAutoCam1->setChecked(false);
    }
    if(ui.cbAutoCam2->isChecked()){
        QFile f;
        QTextStream out(&f);
        f.setFileName("cam2.txt");
        f.open(QIODevice::WriteOnly);
        out << "srt://" + ip + ":2222";
        f.close();
        cam2Url = "srt://" + ip + ":2222";
        ui.cbAutoCam2->setChecked(false);
    }
}

void PCScreen::closeView(){
    if(ui.cbAutoCam1->isChecked())
        ui.cbAutoCam1->setChecked(false);
    if(ui.cbAutoCam2->isChecked())
        ui.cbAutoCam2->setChecked(false);
}

void PCScreen::StartRecord(bool b){
    if(!b){                          //если таймер остановился
        btnStopRecord->setEnabled(true);
        return;
    }
    QString currentTime = QTime::currentTime().toString().replace(":", "_");
    if(threadCam1->isRunning()){
        currentFileCam1 = "video/cam1_" + currentTime + ".mp4";
        camera1->StartRecord(currentFileCam1);    
    }
    if(threadCam2->isRunning()){
        currentFileCam2 = "video/cam2_" + currentTime + ".mp4";
        camera2->StartRecord(currentFileCam2);
    }
    btnPlayLastWithSound1->setEnabled(false);
    btnPlayLastSlowMotion1->setEnabled(false);
    btnPlayLastWithSound2->setEnabled(false);
    btnPlayLastSlowMotion2->setEnabled(false);
    btnPlaySlowMotion->setEnabled(false);
    btnStopRecord->setEnabled(false);
}

void PCScreen::StopRecord(){
    qDebug()<<"1";
    camera1->StopRecord();
    qDebug()<<"2";
    camera2->StopRecord();
    qDebug()<<"3";
    btnPlayLastWithSound1->setEnabled(true);
    btnPlayLastSlowMotion1->setEnabled(true);
    btnPlayLastWithSound2->setEnabled(true);
    btnPlayLastSlowMotion2->setEnabled(true);
    btnPlaySlowMotion->setEnabled(true);
    btnStopRecord->setEnabled(false);
    qDebug()<<"4";
}

void PCScreen::PlayFile(){
    QProcess proc;
    QStringList l;
    l.append(QString("-fs"));
    if(sender()->objectName() == "btnPlayLastWithSound1")
        l.append(currentFileCam1);
    else
        l.append(currentFileCam2);
    proc.start("ffplay", l);
    proc.waitForFinished(-1);
}

void PCScreen::PlaySlowMotion(){
    if(!slowMotionPlayer){
        if(sender()->objectName() == "btnPlayLastSlowMotion1"){
            if(currentFileCam1 == "") return;
            slowMotionPlayer = new PlayerViewer(currentFileCam1);
            connect(slowMotionPlayer, SIGNAL(sigClose()), this, SLOT(closePlayer()));
            btnPlayLastWithSound1->setEnabled(false);
            btnPlayLastSlowMotion1->setEnabled(false);
            btnPlayLastWithSound2->setEnabled(false);
            btnPlayLastSlowMotion2->setEnabled(false);
            btnPlaySlowMotion->setEnabled(false);
        }else{
            if(currentFileCam2 == "") return;
            slowMotionPlayer = new PlayerViewer(currentFileCam2);
            connect(slowMotionPlayer, SIGNAL(sigClose()), this, SLOT(closePlayer()));
            btnPlayLastWithSound1->setEnabled(false);
            btnPlayLastSlowMotion1->setEnabled(false);
            btnPlayLastWithSound2->setEnabled(false);
            btnPlayLastSlowMotion2->setEnabled(false);
            btnPlaySlowMotion->setEnabled(false);
        }
    }
}

void PCScreen::PlaySelectedFile(){
    QString file = QFileDialog::getOpenFileName();
    if(file == "" || !file.endsWith(".mp4"))
        return;
    slowMotionPlayer = new PlayerViewer(file);
    connect(slowMotionPlayer, SIGNAL(sigClose()), this, SLOT(closePlayer()));
}

void PCScreen::closePlayer(){
    slowMotionPlayer->deleteLater();
    btnPlayLastWithSound1->setEnabled(true);
    btnPlayLastSlowMotion1->setEnabled(true);
    btnPlayLastWithSound2->setEnabled(true);
    btnPlayLastSlowMotion2->setEnabled(true);
    btnPlaySlowMotion->setEnabled(true);
}

void PCScreen::finishedCamera(){
    if(sender()->objectName() == "camera1"){
        threadCam1->quit();
        threadCam1->wait();
        if(cbCam1->isChecked())
            cbCam1->toggle();
    }else{
        threadCam2->quit();
        threadCam2->wait();
        if(cbCam2->isChecked())
            cbCam2->toggle();
    }
}

void PCScreen::setCam(){
    QFile f;
    QTextStream out(&f);
    if(sender()->objectName() == "leCam1"){
        f.setFileName("cam1.txt");
        f.open(QIODevice::WriteOnly);
        out << ui.leCam1->text();
        cam1Url = ui.leCam1->text();
    }else{
        f.setFileName("cam2.txt");
        f.open(QIODevice::WriteOnly);
        out << ui.leCam2->text();
        cam2Url = ui.leCam2->text();
    }
    f.close();
}

void PCScreen::turnCamera(bool state){
    if(state){
        if(sender()->objectName() == "cbCam1"){
            camera1->setUrl(cam1Url);
            threadCam1->start();
        }else{
            camera2->setUrl(cam2Url);
            threadCam2->start();
        }
    }
    else{
        if(sender()->objectName() == "cbCam1"){
            camera1->TurnOffCamera();
        }else{
            camera2->TurnOffCamera();
        }
    }
}


void PCScreen::closeEvent(QCloseEvent *){
    if(cbCam1->isChecked()){
        cbCam1->toggle();
        threadCam1->quit();
        threadCam1->wait();
    }
    if(cbCam2->isChecked()){
        cbCam2->toggle();
        threadCam2->quit();
        threadCam2->wait();
    }
    qApp->quit();
}

/*
void PCScreen::process_line(int i, QString s){
    switch (i) {
    case 1:
        fam_red->Text(s);
        break;
    case 2:
        fam_blue->Text(s);
        break;
    case 3:
        rateRed->setRate(s.toInt());
        break;
    case 4:
        rateBlue->setRate(s.toInt());
        break;
    case 5:
        if(s == "1")
            plus_red->setData("+");
        if(s == "2")
            plus_blue->setData("+");
        break;
    case 6:
        cat->setText(s);//setData(s);
        tvScreen->cat->setText(s);
        break;
    case 7:
        actRed->setRate(s.toInt());
        break;
    case 8:
        actBlue->setRate(s.toInt());
        break;
    case 9:
        nv_red->setValue(s);
        tvScreen->nv_red->setValue(s);
        break;
    case 10:
        np_red->setValue(s);
        tvScreen->np_red->setValue(s);
        break;
    case 11:
        np_blue->setValue(s);
        tvScreen->np_blue->setValue(s);
        break;
    case 12:
        nv_blue->setValue(s);
        tvScreen->nv_blue->setValue(s);
        break;
    case 13:
        //mainTimer->showLastTime(s);
        break;
    case 14:
        reg_red->Text(s);
        break;
    case 15:
        reg_blue->Text(s);
        break;
    }

}
*/

/*
PCScreen::~PCScreen(){
    QFile f("start.txt");
    f.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&f);
    out << fam_red->getText() << "\n";
    out << fam_blue->getText() << "\n";
    out << rateRed->text() << "\n";
    out << rateBlue->text() << "\n";
    if(plus_red->getText() == "+")
        out << "1" << "\n";
    else if(plus_blue->getText() == "+")
        out << "2" << "\n";
    else
        out << "0" << "\n";
    out << cat->text() << "\n";
    out << actRed->text() << "\n";
    out << actBlue->text() << "\n";
    out << nv_red->text() << "\n";
    out << np_red->text() << "\n";
    out << np_blue->text() << "\n";
    out << nv_blue->text() << "\n";
    //out << mainTimer->getLastTime() << "\n";
    out << reg_red->getText() << "\n";
    out << reg_blue->getText() << "\n";
}
*/

void PCScreen::setTime(){
    int min = uiTime.dMin->value();
    int sec1 = uiTime.dSec->value();
    int sec2 = uiTime.dSec2->value();
    if(min == 0 && sec1 == 0 && sec2 == 0)
        return;
    mainTimer->setTime(min * 60 + sec1 * 10 + sec2);
    qDebug()<<"setTime";
}

void PCScreen::showView(){
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

void PCScreen::paintEvent(QPaintEvent * ) {
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

void PCScreen::keyPressEvent(QKeyEvent * pe){
    if(pe->key() == Qt::Key_F1)
        frmTime->show();
    else
        sendKey(pe->key());
}

void PCScreen::resizeEvent(QResizeEvent *){
    minimum_height = (height() - 12) / 42;

    percent_height = (height() - 12) / 100;
    //int w = (width() - 12) * 4 / 68;
    //cv_left->setSize(w, w / 2);
    //cv_left->resize(w, w / 2);
    QFont f;
    f.setPixelSize(cat->height() * 0.8);
    cat->setFont(f);
}

void PCScreen::showEvent(QShowEvent *){
	minimum_height = (height() - 12) / 42;

	percent_height = (height() - 12) / 100;
    /*
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

/*
void PCScreen::sendEV_L(QObject * pObj)
{
	QApplication::sendEvent(pObj, ev_L);
}

void PCScreen::sendEV_R(QObject * pObj)
{
	QApplication::sendEvent(pObj, ev_R);
}
*/

void PCScreen::setFlagRed(QString s){
	QString st = "./flags/" + s + ".png";
	QFile * pFile = new QFile(st);
	if (pFile->exists() == true) {
		QPixmap flag = QPixmap();
		flag.load(st);
		flag = flag.scaled(flag_red->size());
		flag_red->setPixmap(flag);
        flag.load(st);
        flag = flag.scaled(tvScreen->flag_red->size());
        tvScreen->flag_red->setPixmap(flag);
	}
}

void PCScreen::setFlagBlue(QString s){
	QString st = "./flags/" + s + ".png";
	QFile * pFile = new QFile(st);
	if (pFile->exists() == true) {
		QPixmap flag = QPixmap();
		flag.load(st);
		flag = flag.scaled(flag_red->size());
		flag_blue->setPixmap(flag);
        flag.load(st);
        flag = flag.scaled(tvScreen->flag_blue->size());
        tvScreen->flag_blue->setPixmap(flag);
	}
}

void PCScreen::setView(void){
	if (ui.rbView1->isChecked()) {
		View = 0;
        rateRed->setViewStyle(0,  ui.sbFrame->value());
        rateBlue->setViewStyle(0, ui.sbFrame->value());
        actRed->setViewStyle(0,   ui.sbFrame->value());
        actBlue->setViewStyle(0,  ui.sbFrame->value());
		fam_red->setViewStyle(0);
		fam_blue->setViewStyle(0);
		reg_red->setViewStyle(0);
		reg_blue->setViewStyle(0);
		fam_red->align = 0;
		fam_blue->align = 0;
		reg_red->align = 0;
		reg_blue->align = 0;
        mainTimer->setFrameShape(QFrame::Box);
		plus_red->setColor("white");
		plus_blue->setColor("white");
        //if (desk->numScreens() != 1) {
            tvScreen->View = 0;
            tvScreen->ball_red->setViewStyle(0,  ui.sbFrame->value());
            tvScreen->ball_blue->setViewStyle(0, ui.sbFrame->value());
            tvScreen->akt_red->setViewStyle(0,   ui.sbFrame->value());
            tvScreen->akt_blue->setViewStyle(0,  ui.sbFrame->value());
            tvScreen->fam_red->setViewStyle(0);
            tvScreen->fam_blue->setViewStyle(0);
            tvScreen->reg_red->setViewStyle(0);
            tvScreen->reg_blue->setViewStyle(0);
            tvScreen->fam_red->align = 0;
            tvScreen->fam_blue->align = 0;
            tvScreen->reg_red->align = 0;
            tvScreen->reg_blue->align = 0;
            tvScreen->sec->setFrameShape(QFrame::Box);
            tvScreen->plus_red->setColor("white");
            tvScreen->plus_blue->setColor("white");
            tvScreen->repaint();
        //}
	}
	else if (ui.rbView2->isChecked()) {
		View = 1;
        rateRed->setViewStyle(1, ui.sbFrame->value());
        rateBlue->setViewStyle(2, ui.sbFrame->value());
        actRed->setViewStyle(1, ui.sbFrame->value());
        actBlue->setViewStyle(2, ui.sbFrame->value());
		fam_red->setViewStyle(1);
		fam_blue->setViewStyle(2);
		reg_red->setViewStyle(1);
		reg_blue->setViewStyle(2);
		fam_red->align = 0;
		fam_blue->align = 0;
		reg_red->align = 0;
		reg_blue->align = 0;
        mainTimer->setFrameShape(QFrame::Box);
		plus_red->setColor("red");
		plus_blue->setColor("blue");
        //if (desk->numScreens() != 1) {
            tvScreen->View = 1;
            tvScreen->ball_red->setViewStyle(1, ui.sbFrame->value());
            tvScreen->ball_blue->setViewStyle(2, ui.sbFrame->value());
            tvScreen->akt_red->setViewStyle(1, ui.sbFrame->value());
            tvScreen->akt_blue->setViewStyle(2, ui.sbFrame->value());
            tvScreen->fam_red->setViewStyle(1);
            tvScreen->fam_blue->setViewStyle(2);
            tvScreen->reg_red->setViewStyle(1);
            tvScreen->reg_blue->setViewStyle(2);
            tvScreen->fam_red->align = 1;
            tvScreen->fam_blue->align = 2;
            tvScreen->reg_red->align = 1;
            tvScreen->reg_blue->align = 2;
            tvScreen->sec->setFrameShape(QFrame::Box);
            tvScreen->plus_red->setColor("red");
            tvScreen->plus_blue->setColor("blue");
            tvScreen->repaint();
        //}
	}
	else {
		View = 1;
        rateRed->setViewStyle(3, ui.sbFrame->value());
        rateBlue->setViewStyle(4, ui.sbFrame->value());
        actRed->setViewStyle(3, ui.sbFrame->value());
        actBlue->setViewStyle(4, ui.sbFrame->value());
		fam_red->setViewStyle(1);
		fam_blue->setViewStyle(2);
		reg_red->setViewStyle(1);
		reg_blue->setViewStyle(2);
		fam_red->align = 1;
		fam_blue->align = 2;
		reg_red->align = 1;
		reg_blue->align = 2;
        mainTimer->setFrameShape(QFrame::NoFrame);
		plus_red->setColor("red");
		plus_blue->setColor("blue");
        //if (desk->numScreens() != 1) {
            tvScreen->View = 1;
            tvScreen->ball_red->setViewStyle(3,  ui.sbFrame->value());
            tvScreen->ball_blue->setViewStyle(4, ui.sbFrame->value());
            tvScreen->akt_red->setViewStyle(3,   ui.sbFrame->value());
            tvScreen->akt_blue->setViewStyle(4,  ui.sbFrame->value());
            tvScreen->fam_red->setViewStyle(1);
            tvScreen->fam_blue->setViewStyle(2);
            tvScreen->reg_red->setViewStyle(1);
            tvScreen->reg_blue->setViewStyle(2);
            tvScreen->fam_red->align =  1;
            tvScreen->fam_blue->align = 2;
            tvScreen->reg_red->align =  1;
            tvScreen->reg_blue->align = 2;
            tvScreen->sec->setFrameShape(QFrame::NoFrame);
            tvScreen->plus_red->setColor("red");
            tvScreen->plus_blue->setColor("blue");
            tvScreen->repaint();
        //}
	}
		repaint();
}

void PCScreen::setFrameWidth(int f){
    mainTimer->setLineWidth(f);
    tvScreen->sec->setLineWidth(f);
	if (View == 0) {

        rateRed->setViewStyle(0, f);
        rateBlue->setViewStyle(0, f);
        actRed->setViewStyle(0, f);
        actBlue->setViewStyle(0, f);
		fam_red->setViewStyle(0);
		fam_blue->setViewStyle(0);
		reg_red->setViewStyle(0);
		reg_blue->setViewStyle(0);
        mainTimer->setFrameShape(QFrame::Box);

        tvScreen->ball_red->setViewStyle(0, f);
        tvScreen->ball_blue->setViewStyle(0, f);
        tvScreen->akt_red->setViewStyle(0, f);
        tvScreen->akt_blue->setViewStyle(0, f);
        tvScreen->fam_red->setViewStyle(0);
        tvScreen->fam_blue->setViewStyle(0);
        tvScreen->reg_red->setViewStyle(0);
        tvScreen->reg_blue->setViewStyle(0);
        tvScreen->sec->setFrameShape(QFrame::Box);
	}
	else {
        rateRed->setViewStyle(1, f);
        rateBlue->setViewStyle(2, f);
        actRed->setViewStyle(1, f);
        actBlue->setViewStyle(2, f);
		fam_red->setViewStyle(1);
		fam_blue->setViewStyle(2);
		reg_red->setViewStyle(1);
		reg_blue->setViewStyle(2);
        mainTimer->setFrameShape(QFrame::Box);

        tvScreen->ball_red->setViewStyle(1, f);
        tvScreen->ball_blue->setViewStyle(2, f);
        tvScreen->akt_red->setViewStyle(1, f);
        tvScreen->akt_blue->setViewStyle(2, f);
        tvScreen->fam_red->setViewStyle(1);
        tvScreen->fam_blue->setViewStyle(2);
        tvScreen->reg_red->setViewStyle(1);
        tvScreen->reg_blue->setViewStyle(2);
        tvScreen->sec->setFrameShape(QFrame::Box);
		//sec->setLineWidth(5);
	}
	
	repaint();
}

void PCScreen::setSpace(int s){
    //grid->setSpacing(s);
    tvScreen->grid->setSpacing(s);
	repaint();
}

void PCScreen::setSec(int m) {
    //qDebug()<<m;
	//grid->setMargin(m);
	//repaint();
    //grid->removeWidget(mainTimer);
    tvScreen->grid->removeWidget(tvScreen->sec);
    if(m == 3){
        //grid->addWidget(mainTimer, 24, 24, 13, 20);
        tvScreen->grid->addWidget(tvScreen->sec, 24, 24, 13, 20);
    }else if(m == 2){
        //grid->addWidget(mainTimer, 25, 24, 11, 20);
        tvScreen->grid->addWidget(tvScreen->sec, 25, 24, 11, 20);
    }else{
        //grid->addWidget(mainTimer, 26, 24, 9, 20);
        tvScreen->grid->addWidget(tvScreen->sec, 26, 24, 9, 20);
    }
}

void PCScreen::changeSize() {
    int i = 0;
    if(sender()->objectName() == "btnNameDown") i = 1;
    if(sender()->objectName() == "btnRegUp")    i = 2;
    if(sender()->objectName() == "btnRegDown")  i = 3;

	if (i == 0) {
		if (HEIGHT_FAMILY < 5) {
			HEIGHT_FAMILY += 1;
            /*
			grid->setRowMinimumHeight(0, minimum_height + HEIGHT_FAMILY * percent_height / 6);
			grid->setRowMinimumHeight(1, minimum_height + HEIGHT_FAMILY * percent_height / 6);
			grid->setRowMinimumHeight(2, minimum_height + HEIGHT_FAMILY * percent_height / 6);
			grid->setRowMinimumHeight(3, minimum_height + HEIGHT_FAMILY * percent_height / 6);
			grid->setRowMinimumHeight(4, minimum_height + HEIGHT_FAMILY * percent_height / 6);
			grid->setRowMinimumHeight(5, minimum_height + HEIGHT_FAMILY * percent_height / 6);
            */
            //if (desk->numScreens() != 1) {
                tvScreen->grid->setRowMinimumHeight(0, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
                tvScreen->grid->setRowMinimumHeight(1, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
                tvScreen->grid->setRowMinimumHeight(2, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
                tvScreen->grid->setRowMinimumHeight(3, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
                tvScreen->grid->setRowMinimumHeight(4, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
                tvScreen->grid->setRowMinimumHeight(5, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);

            //}
			if (HEIGHT_FAMILY == -5) {

				fam_red->setVisible(true);
				fam_blue->setVisible(true);
                grid->addWidget(fam_red, 0, 0, 4, 34);
                grid->addWidget(fam_blue, 0, 34, 4, 34);
                //if (desk->numScreens() != 1) {
                    tvScreen->fam_red->setVisible(true);
                    tvScreen->fam_blue->setVisible(true);
                    tvScreen->grid->addWidget(tvScreen->fam_red, 0, 34, 6, 34);
                    tvScreen->grid->addWidget(tvScreen->fam_blue, 0, 0, 6, 34);
                //}
			}
		}
	}
	else if (i == 1) {
		if (HEIGHT_FAMILY > -6) {
			HEIGHT_FAMILY -= 1;
			if (HEIGHT_FAMILY != -6) {
                /*
				grid->setRowMinimumHeight(0, minimum_height + HEIGHT_FAMILY * percent_height / 6);
				grid->setRowMinimumHeight(1, minimum_height + HEIGHT_FAMILY * percent_height / 6);
				grid->setRowMinimumHeight(2, minimum_height + HEIGHT_FAMILY * percent_height / 6);
				grid->setRowMinimumHeight(3, minimum_height + HEIGHT_FAMILY * percent_height / 6);
				grid->setRowMinimumHeight(4, minimum_height + HEIGHT_FAMILY * percent_height / 6);
				grid->setRowMinimumHeight(5, minimum_height + HEIGHT_FAMILY * percent_height / 6);
                */
                //if (desk->numScreens() != 1) {
                    tvScreen->grid->setRowMinimumHeight(0, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
                    tvScreen->grid->setRowMinimumHeight(1, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
                    tvScreen->grid->setRowMinimumHeight(2, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
                    tvScreen->grid->setRowMinimumHeight(3, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
                    tvScreen->grid->setRowMinimumHeight(4, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
                    tvScreen->grid->setRowMinimumHeight(5, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
                //}
			}
			else {
                /*
				grid->setRowMinimumHeight(0, 0);
				grid->setRowMinimumHeight(1, 0);
				grid->setRowMinimumHeight(2, 0);
				grid->setRowMinimumHeight(3, 0);
				grid->setRowMinimumHeight(4, 0);
				grid->setRowMinimumHeight(5, 0);
                */
				fam_red->setVisible(false);
				fam_blue->setVisible(false);
				grid->removeWidget(fam_red);
				grid->removeWidget(fam_blue);
                //if (desk->numScreens() != 1) {
                    tvScreen->grid->setRowMinimumHeight(0, 0);
                    tvScreen->grid->setRowMinimumHeight(1, 0);
                    tvScreen->grid->setRowMinimumHeight(2, 0);
                    tvScreen->grid->setRowMinimumHeight(3, 0);
                    tvScreen->grid->setRowMinimumHeight(4, 0);
                    tvScreen->grid->setRowMinimumHeight(5, 0);
                    tvScreen->fam_red->setVisible(false);
                    tvScreen->fam_blue->setVisible(false);
                    tvScreen->grid->removeWidget(tvScreen->fam_red);
                    tvScreen->grid->removeWidget(tvScreen->fam_blue);

                //}
			}

		}
	}
	else if (i == 2) {
		if (HEIGHT_REGION < 5) {
			HEIGHT_REGION += 1;
            /*
			grid->setRowMinimumHeight(37, minimum_height + HEIGHT_REGION * percent_height / 5);
			grid->setRowMinimumHeight(38, minimum_height + HEIGHT_REGION * percent_height / 5);
			grid->setRowMinimumHeight(39, minimum_height + HEIGHT_REGION * percent_height / 5);
			grid->setRowMinimumHeight(40, minimum_height + HEIGHT_REGION * percent_height / 5);
			grid->setRowMinimumHeight(41, minimum_height + HEIGHT_REGION * percent_height / 5);
            */
            //if (desk->numScreens() != 1) {
                tvScreen->grid->setRowMinimumHeight(37, tvScreen->minimum_height + HEIGHT_REGION * tvScreen->percent_height / 5);
                tvScreen->grid->setRowMinimumHeight(38, tvScreen->minimum_height + HEIGHT_REGION * tvScreen->percent_height / 5);
                tvScreen->grid->setRowMinimumHeight(39, tvScreen->minimum_height + HEIGHT_REGION * tvScreen->percent_height / 5);
                tvScreen->grid->setRowMinimumHeight(40, tvScreen->minimum_height + HEIGHT_REGION * tvScreen->percent_height / 5);
                tvScreen->grid->setRowMinimumHeight(41, tvScreen->minimum_height + HEIGHT_REGION * tvScreen->percent_height / 5);
            //}
			if (HEIGHT_REGION == -5) {
				reg_red->setVisible(true);
				reg_blue->setVisible(true);
                grid->addWidget(reg_red, 38, 0, 4, 34);
                grid->addWidget(reg_blue, 38, 34, 4, 34);

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
                    tvScreen->reg_blue->setVisible(true);
                    tvScreen->reg_red->setVisible(true);
                    tvScreen->grid->addWidget(tvScreen->reg_red, 37, 34, 5, 34);
                    tvScreen->grid->addWidget(tvScreen->reg_blue, 37, 0, 5, 34);

                    tvScreen->flag_red->setVisible(true);
                    tvScreen->flag_blue->setVisible(true);
                    tvScreen->grid->addWidget(tvScreen->flag_red, 29, 14, 8, 10);
                    tvScreen->grid->addWidget(tvScreen->flag_blue, 29, 44, 8, 10);

                    tvScreen->grid->removeWidget(tvScreen->np_blue);
                    tvScreen->grid->removeWidget(tvScreen->np_red);
                    tvScreen->grid->addWidget(tvScreen->np_red, 24, 44, 5, 5);
                    tvScreen->grid->addWidget(tvScreen->np_blue, 24, 19, 5, 5);

                    tvScreen->grid->removeWidget(tvScreen->nv_blue);
                    tvScreen->grid->removeWidget(tvScreen->nv_red);
                    tvScreen->grid->addWidget(tvScreen->nv_red, 24, 49, 5, 5);
                    tvScreen->grid->addWidget(tvScreen->nv_blue, 24, 14, 5, 5);
                //}
			}
		}
	}
	else {
		if (HEIGHT_REGION > -6) {
			HEIGHT_REGION -= 1;
			if (HEIGHT_REGION != -6) {
                /*
				grid->setRowMinimumHeight(37, minimum_height + HEIGHT_REGION * percent_height / 5);
				grid->setRowMinimumHeight(38, minimum_height + HEIGHT_REGION * percent_height / 5);
				grid->setRowMinimumHeight(39, minimum_height + HEIGHT_REGION * percent_height / 5);
				grid->setRowMinimumHeight(40, minimum_height + HEIGHT_REGION * percent_height / 5);
				grid->setRowMinimumHeight(41, minimum_height + HEIGHT_REGION * percent_height / 5);
                */
                //if (desk->numScreens() != 1) {
                    tvScreen->grid->setRowMinimumHeight(37, tvScreen->minimum_height + HEIGHT_REGION * tvScreen->percent_height / 5);
                    tvScreen->grid->setRowMinimumHeight(38, tvScreen->minimum_height + HEIGHT_REGION * tvScreen->percent_height / 5);
                    tvScreen->grid->setRowMinimumHeight(39, tvScreen->minimum_height + HEIGHT_REGION * tvScreen->percent_height / 5);
                    tvScreen->grid->setRowMinimumHeight(40, tvScreen->minimum_height + HEIGHT_REGION * tvScreen->percent_height / 5);
                    tvScreen->grid->setRowMinimumHeight(41, tvScreen->minimum_height + HEIGHT_REGION * tvScreen->percent_height / 5);
                //}
			}
			else {
                /*
				grid->setRowMinimumHeight(37, 0);
				grid->setRowMinimumHeight(38, 0);
				grid->setRowMinimumHeight(39, 0);
				grid->setRowMinimumHeight(40, 0);
				grid->setRowMinimumHeight(41, 0);
                */
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
                    tvScreen->grid->setRowMinimumHeight(37, 0);
                    tvScreen->grid->setRowMinimumHeight(38, 0);
                    tvScreen->grid->setRowMinimumHeight(39, 0);
                    tvScreen->grid->setRowMinimumHeight(40, 0);
                    tvScreen->grid->setRowMinimumHeight(41, 0);
                    tvScreen->reg_blue->setVisible(false);
                    tvScreen->reg_red->setVisible(false);
                    tvScreen->grid->removeWidget(tvScreen->reg_blue);
                    tvScreen->grid->removeWidget(tvScreen->reg_red);

                    tvScreen->grid->removeWidget(tvScreen->flag_red);
                    tvScreen->grid->removeWidget(tvScreen->flag_blue);
                    tvScreen->flag_red->setVisible(false);
                    tvScreen->flag_blue->setVisible(false);

                    tvScreen->grid->removeWidget(tvScreen->np_blue);
                    tvScreen->grid->removeWidget(tvScreen->np_red);
                    tvScreen->grid->addWidget(tvScreen->np_red, 28, 44, 5, 5);
                    tvScreen->grid->addWidget(tvScreen->np_blue, 28, 19, 5, 5);

                    tvScreen->grid->removeWidget(tvScreen->nv_red);
                    tvScreen->grid->removeWidget(tvScreen->nv_blue);
                    tvScreen->grid->addWidget(tvScreen->nv_red, 28, 49, 5, 5);
                    tvScreen->grid->addWidget(tvScreen->nv_blue, 28, 14, 5, 5);
                //}
			}
		}
	}


    //repaint();
    //tvScreen->grid->removeWidget(tvScreen->rV);
    //tvScreen->grid->addWidget(tvScreen->rV, 0, 0, 42, 68);
    //tvScreen->repaint();
    //tvScreen->rV->raise();
    //tvScreen->rV->setGeometry(0, 0, tvScreen->width(), tvScreen->height());
}

void PCScreen::drawTvScreenshot(){
    if(formView->isVisible()){
        QPixmap pix = tvScreen->grab();
        pix = pix.scaled(ui.lblTvscreen->width(), ui.lblTvscreen->height());
        ui.lblTvscreen->setPixmap(pix);
    }
}

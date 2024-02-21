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
#include "QAction"

#include <math.h>



#include "category.h"

//#include <QHostAddress>
#include <QNetworkInterface>
//#include <QHostInfo>
#include <QRadioButton>

WidgetFilter::WidgetFilter(QObject* pobj) : QObject(pobj){
    qDebug()<<"constructor event";
}

WidgetFilter::~WidgetFilter(){
    qDebug()<<"destructor event";
}

bool WidgetFilter::eventFilter(QObject* , QEvent* pe){
    if(pe->type() == QEvent::Close){
        emit sigClose();
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

PCScreen::PCScreen(MainWindow* mw, QWidget * parent) : QWidget(parent){
    //ev_L = new MyEvent(200);
    //ev_R = new MyEvent(201);

    //newSportsman = new NewSportsman;

    mainwin = mw;

    address = "";
    remoteAddress = new QHostAddress;
    s_udp = new QUdpSocket(this);
    s_udp->bind(2425 );
    flagUdp = 0;
    connect(s_udp, SIGNAL(readyRead()), this, SLOT(slotProcessDatagrams()));
    udpTimer = new QTimer(this);
    connect(udpTimer, SIGNAL(timeout()), this, SLOT(udpSend()));
    //qDebug()<<s_udp->localAddress().toString();

	col_red = "white";
	col_blue = "white";

#ifdef APP_LAUNCH_FROM_IDE
    fileSettings = "settings.ini";
#else
   fileSettings = "bin/settings.ini";
#endif


    settings = new QSettings(fileSettings, QSettings::IniFormat);
    settings->beginGroup("height");

    HEIGHT_REGION = settings->value("HEIGHT_REGION", 0).toInt();
    HEIGHT_FAMILY = settings->value("HEIGHT_FAMILY", 0).toInt();
    HEIGHT_NEXT_FAMILY = settings->value("HEIGHT_NEXT_FAMILY", 0).toInt();

    settings->endGroup();

	minimum_height = 0;
	percent_height = 0;



    rateRed = new Rate(this);
    rateRed->setFrameShape(QFrame::Box);
    rateRed->setObjectName("ball_red");

    rateBlue = new Rate(this);
    rateBlue->setFrameShape(QFrame::Box);
    rateBlue->setObjectName("ball_blue");

    //actRed = new Rate(this);
    //actRed->setFrameShape(QFrame::Box);
    //actRed->setObjectName("akt_red");

    //actBlue = new Rate(this);
    //actBlue->setFrameShape(QFrame::Box);
    //actBlue->setObjectName("akt_blue");

	fam_red = new Fam(col_red, "", 63,"",this);
	fam_red->setObjectName("fam_red");
	reg_red = new Fam(col_red, "", 10, "Lucida Console");
	reg_red->setObjectName("reg_red");

	fam_blue = new Fam(col_blue, "", 63,"",this);
	fam_blue->setObjectName("fam_blue");
	reg_blue = new Fam(col_blue, "", 10, "Lucida Console");
	reg_blue->setObjectName("reg_blue");
	
    fam_next_red = new Fam(col_red, "", 63,"",this);
    fam_next_red->setObjectName("fam_next_red");

    fam_next_blue = new Fam(col_blue, "", 63,"",this);
    fam_next_blue->setObjectName("fam_next_blue");

    cbAddDisp = new QCheckBox("дополнительный\nдисплей", this);     //передача данных для дополнительного дисплея
    cbAddDisp->setStyleSheet("color: white");
    connect(cbAddDisp, SIGNAL(stateChanged(int)), this, SLOT(addDisplay(int)));

//    QPushButton* addSportsman = new QPushButton(u8"Добавить\nспортсмена", this);
//    addSportsman->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
//    //addSportsman->setStyleSheet("color: red; font: bold " + QString::number(round(doctor->height() / 2)) + "px;");
//    connect(addSportsman, SIGNAL(clicked()), newSportsman, SLOT(show()));

    IconButton* doctor = new IconButton(":/images/doctor.svg");
    //doctor->setObjectName("btnParter_red");
    doctor->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    //doctor->setStyleSheet("color: red; font: bold " + QString::number(round(doctor->height() / 2)) + "px;");
    connect(doctor, SIGNAL(clicked()), this, SLOT(turnDoctor()));

    IconButton * btnParter_red = new IconButton(":/images/parter_red.svg");
	btnParter_red->setObjectName("btnParter_red");
	btnParter_red->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    //btnParter_red->setStyleSheet("color: red; font: bold " + QString::number(round(btnParter_red->height() / 2)) + "px;");
    //btnParter_red->setFocusPolicy(Qt::NoFocus);
    //btnParter_red->setIcon(QIcon(":/images/parter_red.png"));
    //btnParter_red->setIconSize(QSize(btnParter_red->width(), btnParter_red->height()));
	
    IconButton * btnTime = new IconButton(":/images/time.svg");
    btnTime->setObjectName("btnTime");
	btnTime->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	//btnTime->setStyleSheet("color: green; font: bold " + QString::number(round(btnTime->height() / 2)) + "px;");
    //btnTime->setFocusPolicy(Qt::NoFocus);

    IconButton * btnParter_blue = new IconButton(":/images/parter_blue.svg");
	btnParter_blue->setObjectName("btnParter_blue");
	btnParter_blue->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    //btnParter_blue->setStyleSheet("color: blue; font: bold " + QString::number(round(btnParter_blue->height() / 2)) + "px;");
    //btnParter_blue->setFocusPolicy(Qt::NoFocus);
	
    IconButton * btnTehTime_red = new IconButton(":/images/ttech_red.svg");
	btnTehTime_red->setObjectName("btnTehTime_red");
	btnTehTime_red->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	btnTehTime_red->setStyleSheet("color: red; font: bold " + QString::number(round(btnTehTime_red->height() / 2)) + "px;");
    //btnTehTime_red->setFocusPolicy(Qt::NoFocus);

    IconButton * btnSettings = new IconButton(":/images/sportsmens.svg");
	btnSettings->setObjectName("btnSettings");
	btnSettings->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    //btnSettings->setStyleSheet("color: green; font: bold " + QString::number(round(btnSettings->height() / 2)) + "px;");
    connect(btnSettings, SIGNAL(clicked()), this, SLOT(showListSportsmens()));
    //btnSettings->setFocusPolicy(Qt::NoFocus);
	
    IconButton * btnTehTime_blue = new IconButton(":/images/ttech_blue.svg");
	btnTehTime_blue->setObjectName("btnTehTime_blue");
	btnTehTime_blue->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    //btnTehTime_blue->setStyleSheet("color: blue; font: bold " + QString::number(round(btnTehTime_blue->height() / 2)) + "px;");
    //btnTehTime_blue->setFocusPolicy(Qt::NoFocus);

    IconButton * btnPlus_red = new IconButton(":/images/plus_red.svg");
	btnPlus_red->setObjectName("btnPlus_red");
	btnPlus_red->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	btnPlus_red->setStyleSheet("color: red; font: bold " + QString::number(btnPlus_red->height()) + "px;");
    //btnPlus_red->setFocusPolicy(Qt::NoFocus);

    IconButton * btnPlus_blue = new IconButton(":/images/plus_blue.svg");
	btnPlus_blue->setObjectName("btnPlus_blue");
	btnPlus_blue->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	btnPlus_blue->setStyleSheet("color: blue; font: bold " + QString::number(btnPlus_blue->height()) + "px;");
    //btnPlus_blue->setFocusPolicy(Qt::NoFocus);

    //QPushButton * btnView = new QPushButton("НАСТРОЙКИ", this);
	//btnView->setObjectName("btnPlus_blue");
    //btnView->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    //btnView->setStyleSheet("font: bold " + QString::number(round(btnView->height() / 2)) + "px;");
    //btnView->setFocusPolicy(Qt::NoFocus);

    plus_red = new Plus(col_red, this);
	plus_red->setObjectName("plus_red");
    plus_red->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    plus_blue = new Plus(col_blue, this);
	plus_blue->setObjectName("plus_blue");
    plus_blue->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    mainTimer = new LCDTimer(this);
    mainTimer->setObjectName("mainTimer");
    //connect(btnTime, SIGNAL(clicked()), mainTimer, SLOT(StartStop()));



    sec_doctor = new LCDStopwatch(this, "2:00", QColor(255, 255, 0), QColor(255, 255, 0), true, true);
    sec_doctor->hide();

    sec_red = new LCDStopwatch(this, "0:20", QColor(255, 0, 0), QColor(255, 102, 102), true, true);
	sec_red->setObjectName("sec_red");
	sec_red->hide();

    sec_blue = new LCDStopwatch(this, "0:20", QColor(0, 0, 255), QColor(102, 102, 255), true, true);
	sec_blue->setObjectName("sec_blue");
	sec_blue->hide();

    sec_red_t = new LCDStopwatch(this, "2:00", QColor(255, 0, 0), QColor(255, 102, 102), true, true);
    sec_red_t->setObjectName("sec_red_t");
    sec_red_t->hide();

    sec_blue_t = new LCDStopwatch(this, "2:00", QColor(0, 0, 255), QColor(102, 102, 255), true, true);
    sec_blue_t->setObjectName("sec_blue_t");
    sec_blue_t->hide();


	np_red = new NP();
	np_red->setObjectName("np_red");
	//np_red->sbros();
	np_blue = new NP();
	np_blue->setObjectName("np_blue");
	//np_blue->sbros();

    cat = new QPushButton(this);//("yellow", this);
    cat->setAutoFillBackground(true);
    cat->setStyleSheet("background-color: black; color: white; text-align: center");
    cat->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    age = new QPushButton(this);//("yellow", this);
    age->setAutoFillBackground(true);
    age->setStyleSheet("background-color: black; color: white; text-align: center");
    age->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    choosingNames = new ChoosingNames;

    lf = new ListFamily(this);
    lf->setObjectName("lf");
    connect(lf->weight, SIGNAL(currentTextChanged(QString)), this, SLOT(setCat(QString)));
    connect(lf->age, SIGNAL(currentTextChanged(QString)), this, SLOT(setAge(QString)));
    connect(lf, SIGNAL(sig_hide(QString, QString, QString, QString)), this, SLOT(HIDE(QString, QString, QString, QString)));

    choosingNames->setNames(lf->getSportsmens());
    choosingNames->setAge(lf->lAge);
    choosingNames->setWeight(lf->lWeight);

	formView = new QWidget;
    ui.setupUi(formView);
    WidgetFilter* wf = new WidgetFilter(formView);
    formView->installEventFilter(wf);
    connect(wf, SIGNAL(sigClose()), this, SLOT(closeView()));


    //connect(formView, SIGNAL())

    connect(ui.btnNameDown,     SIGNAL(clicked()), this, SLOT(changeSize()));
    connect(ui.btnNameUp,       SIGNAL(clicked()), this, SLOT(changeSize()));
    connect(ui.btnNextNameDown, SIGNAL(clicked()), this, SLOT(changeSize()));
    connect(ui.btnNextNameUp,   SIGNAL(clicked()), this, SLOT(changeSize()));
    connect(ui.btnRegDown,      SIGNAL(clicked()), this, SLOT(changeSize()));
    connect(ui.btnRegUp,        SIGNAL(clicked()), this, SLOT(changeSize()));

    ui.cmbFont->addItems({"10", "12", "14", "16", "18", "20", "23", "26", "29", "34", "39", "45", "50", "55", "60",
                          "65", "70", "75", "80", "85", "90", "100", "110", "120", "130", "140", "150", "170", "180", "190", "200", "210", "220", "230"});
    connect(ui.cmbFont,         SIGNAL(currentTextChanged(QString)), this, SLOT(changeFontWeight(QString)));

    //ui.cmbFam->addItems({"ФАМИЛИЯ ИМЯ", "ФАМИЛИЯ И.", "ФАМИЛИЯ"});
    //ui.cmbFont->setObjectName("viewName");
    //connect(ui.cmbFam,         SIGNAL(currentIndexChanged(int)), this, SIGNAL(changeViewName(int)));

    frmTime = new QWidget;
    uiTime.setupUi(frmTime);

    //connect(btnView, SIGNAL(clicked(bool)), this, SLOT(showView()));
    connect(mainwin->winSettings, SIGNAL(triggered()), this, SLOT(showView()));

    //connect(ui.rbView1, SIGNAL(toggled(bool)), this, SLOT(setView(void)));
    //connect(ui.rbView2, SIGNAL(toggled(bool)), this, SLOT(setView(void)));
    //connect(ui.rbView3, SIGNAL(toggled(bool)), this, SLOT(setView(void)));
    //connect(ui.sbFrame, SIGNAL(valueChanged(int)), this, SLOT(setFrameWidth(int)));
    //connect(ui.sbSpacing, SIGNAL(valueChanged(int)), this, SLOT(setSpace(int)));
	connect(ui.sbSec, SIGNAL(valueChanged(int)), this, SLOT(setSec(int)));

    viewCam1 = new CameraViewer;
    viewCam1->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    viewCam2 = new CameraViewer;
    viewCam2->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    btnPlayLastWithSound1 = new QPushButton("Просмотр\nсо звуком");
    btnPlayLastWithSound1->setObjectName("btnPlayLastWithSound1");
    btnPlayLastWithSound1->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    btnPlayLastWithSound1->setStyleSheet("font-size: 8pt");
    connect(btnPlayLastWithSound1, SIGNAL(clicked()), SLOT(PlayFile()));

    btnPlayLastWithSound2 = new QPushButton("Просмотр\nсо звуком");
    btnPlayLastWithSound2->setObjectName("btnPlayLastWithSound2");
    btnPlayLastWithSound2->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    btnPlayLastWithSound2->setStyleSheet("font-size: 8pt");
    connect(btnPlayLastWithSound2, SIGNAL(clicked()), SLOT(PlayFile()));

    btnPlayLastSlowMotion1 = new QPushButton("Просмотр\nзамедленный");
    btnPlayLastSlowMotion1->setObjectName("btnPlayLastSlowMotion1");
    btnPlayLastSlowMotion1->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    btnPlayLastSlowMotion1->setStyleSheet("font-size: 8pt");
    connect(btnPlayLastSlowMotion1, SIGNAL(clicked()), SLOT(PlaySlowMotion()));

    btnPlayLastSlowMotion2 = new QPushButton("Просмотр\nзамедленный");
    btnPlayLastSlowMotion2->setObjectName("btnPlayLastSlowMotion2");
    btnPlayLastSlowMotion2->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    btnPlayLastSlowMotion2->setStyleSheet("font-size: 8pt");
    connect(btnPlayLastSlowMotion2, SIGNAL(clicked()), SLOT(PlaySlowMotion()));

    btnPlaySlowMotion = new QPushButton("Просмотр с выбором файла");
    btnPlaySlowMotion->setObjectName("btnPlaySlowMotion");
    btnPlaySlowMotion->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    btnPlaySlowMotion->setStyleSheet("font-size: 8pt");
    connect(btnPlaySlowMotion, SIGNAL(clicked()), SLOT(PlaySelectedFile()));

    btnStopRecord = new QPushButton("Стоп запись");
    btnStopRecord->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    btnStopRecord->setStyleSheet("font-size: 8pt");
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

    lbl = new QLabel("Последний записанный файл");
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

    QLabel* lbl3 = new QLabel("Добавить спортсмена - 'F2'");
    //lbl3->setAlignment(Qt::);
    lbl3->setStyleSheet("color: white; font-size: 12pt");

    //lblCpuUsage = new QLabel;
    //lblCpuUsage->setAlignment(Qt::AlignCenter);
    //lblCpuUsage->setStyleSheet("background-color: black; color: white; font-size: 12pt");

    //lblTv.hide();

    //QPushButton * btnSetTime = new QPushButton("Настройка времени", this);
    //btnSetTime->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    //connect(btnSetTime, SIGNAL(clicked()), this, SLOT(setTimeFight()));
    connect(mainwin->time, SIGNAL(triggered()), this, SLOT(setTimeFight()));

    //QPushButton * btnClose = new QPushButton("Выход", this);
    //btnClose->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    //connect(btnClose, SIGNAL(clicked()), this, SLOT(closeTablo()));
    connect(mainwin->closeProg, SIGNAL(triggered()), this, SLOT(closeTablo()));

    connect(mainwin->actSbros, SIGNAL(triggered()), this, SLOT(resetTablo()));

//    QPushButton * btnReset = new QPushButton("Сброс", this);
//    btnReset->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
//    connect(btnReset, SIGNAL(clicked()), this, SLOT(resetTablo()));

	grid = new QGridLayout(this);
	grid->setObjectName("grid");
	//spacing = 6;
	//margin = 6;
    //сетка 68х42
    grid->setSpacing(6);
	grid->setMargin(6);
    grid->addWidget(fam_red,                0,  0,  4,  34);
    grid->addWidget(fam_blue,               0,  34, 4,  34);
    grid->addWidget(reg_red,                4, 0,  4,  34);
    grid->addWidget(reg_blue,               4, 34, 4,  34);

    grid->addWidget(rateRed,                8,  0,  20, 24);
    grid->addWidget(rateBlue,               8,  44, 20, 24);
    //grid->addWidget(actRed,                 28, 0,  14, 16);
    //grid->addWidget(actBlue,                28, 52, 14, 16);//////////

    grid->addWidget(np_red,                 32,  9, 6,  7);
    grid->addWidget(np_blue,                32, 53, 6,  7);

    grid->addWidget(btnTehTime_red,         8,  24, 2,  6);
    grid->addWidget(btnSettings,            8,  30, 2,  8);
    grid->addWidget(btnTehTime_blue,        8,  38, 2,  6);

    grid->addWidget(btnPlus_red,            10,  24, 2,  2);
    //grid->addWidget(lblCpuUsage,            10,  33, 2,  2);
    grid->addWidget(btnPlus_blue,           10,  42, 2,  2);

    grid->addWidget(plus_red,               9,   19, 4,  4);
    grid->addWidget(plus_blue,              9,   45, 4,  4);

    grid->addWidget(age,                    12, 26, 2,  7);
    grid->addWidget(cat,                    12, 35, 2,  7);

    //grid->addWidget(btnView,                10, 24, 2,  6);

    //grid->addWidget(lbl2,                   12, 24, 2,  20);
    //grid->addWidget(btnSetTime,             12, 24, 2,  6);
    //grid->addWidget(btnClose,               12, 32, 2,  4);
    //grid->addWidget(btnReset,               29, 20, 2,  4);

    grid->addWidget(btnParter_red,          26, 24, 2,  6);
    grid->addWidget(btnTime,                26, 31, 2,  6);
    grid->addWidget(btnParter_blue,         26, 38, 2,  6);


    //grid->addWidget(btnStartPlay,         20, 38, 2,  3);

    grid->addWidget(mainTimer,              28, 24, 14, 20);
    grid->addWidget(sec_doctor,             28, 24, 14, 20);

    grid->addWidget(sec_red,                11,  0,  12, 24);
    grid->addWidget(sec_blue,               11,  44, 12, 24);

    grid->addWidget(sec_red_t,              11,  0,  12, 24);
    grid->addWidget(sec_blue_t,             11,  44, 12, 24);

    grid->addWidget(viewCam1,               14, 24,  6, 10);
    grid->addWidget(btnPlayLastWithSound1,  20, 24,  2,  5);
    grid->addWidget(btnPlayLastSlowMotion1, 20, 29,  2,  5);
    grid->addWidget(btnPlaySlowMotion,      24, 24,  2, 10);
    grid->addWidget(cbCam1,                 19, 25,  1,  8);
    grid->addWidget(viewCam2,               14, 34,  6, 10);
    grid->addWidget(btnPlayLastWithSound2,  20, 34,  2,  5);
    grid->addWidget(btnPlayLastSlowMotion2, 20, 39,  2,  5);
    grid->addWidget(btnStopRecord,          24, 34,  2, 10);
    grid->addWidget(cbCam2,                 19 , 35,  1, 8);


    grid->addWidget(lbl,                    22, 24,  2, 20);


    grid->addWidget(fam_next_red,           42,  0,  4,  34);
    grid->addWidget(fam_next_blue,          42,  34, 4,  34);

    grid->addWidget(doctor,                 29,  45, 2,   6);

    grid->addWidget(cbAddDisp,              28,  4, 4,   20);
    //grid->addWidget(lbl3,                   38,  4, 2,   20);
    //grid->addWidget(&lblTv,                 21,  0, 21, 34);

    //grid->addWidget(rV,              0, 13,  23,  42);

    //connect(rV, SIGNAL(end_replay()), SLOT(removeReplay()));
    //rV->hide();
    //minimum_height_family = (height() - 12) / 42;
	//minimum_height_region = minimum_height_family;

    btnTime->setStyleSheet("color: green; font: bold " + QString::number(round(btnTime->height() / 2)) + "px;");

    lblEndTimer = new EndTime(this);
    lblEndTimer->setObjectName("lblEndTimer");

    //desk = QApplication::desktop();

    //if (desk->numScreens() == 2){
//    if(QGuiApplication::screens().count() == 2){
//        showFullScreen();
//    }else{
//        show();
//        setGeometry(QApplication::desktop()->availableGeometry(this).width() / 2, QApplication::desktop()->availableGeometry(this).height() / 2,
//                    QApplication::desktop()->availableGeometry(this).width() / 2, QApplication::desktop()->availableGeometry(this).height() / 2);
//    }

    //if (desk->numScreens() == 1) {
    if(QGuiApplication::screens().count() == 1){
        QMessageBox::information(this, "ВНИМАНИЕ!",
        "Подключите к компьютеру дисплей в режиме \"Расширенный рабочий стол!\"",
		QMessageBox::Ok);
	}
    //else {
        tvScreen = new TVScreen;
        //tvScreen->setObjectName("tvScreen");
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

        connect(rateRed,	SIGNAL(sigRate(int)),		 tvScreen->ball_red,	  SLOT(setRate(int)));
        connect(rateBlue,	SIGNAL(sigRate(int)),		 tvScreen->ball_blue,     SLOT(setRate(int)));
        //connect(actRed,     SIGNAL(sigRate(int)),		 tvScreen->akt_red,	      SLOT(setRate(int)));
        //connect(actBlue,	SIGNAL(sigRate(int)),		 tvScreen->akt_blue,	  SLOT(setRate(int)));

        connect(np_red,		SIGNAL(ball(QString)),	     tvScreen->np_red,		  SLOT(setValue(QString)));
        connect(np_blue,	SIGNAL(ball(QString)),	     tvScreen->np_blue,	      SLOT(setValue(QString)));

        connect(plus_red,   SIGNAL(textChange(QString)), tvScreen->plus_red,      SLOT(setData(QString)));
        connect(plus_blue,  SIGNAL(textChange(QString)), tvScreen->plus_blue,     SLOT(setData(QString)));

        connect(fam_red,       SIGNAL(sigText(QString)), tvScreen->fam_red,       SLOT(Text(QString)));
        connect(fam_blue,      SIGNAL(sigText(QString)), tvScreen->fam_blue,      SLOT(Text(QString)));
        connect(reg_red,       SIGNAL(sigText(QString)), tvScreen->reg_red,       SLOT(Text(QString)));
        connect(reg_blue,      SIGNAL(sigText(QString)), tvScreen->reg_blue,      SLOT(Text(QString)));
        connect(fam_next_red,  SIGNAL(sigText(QString)), tvScreen->fam_next_red,  SLOT(Text(QString)));
        connect(fam_next_blue, SIGNAL(sigText(QString)), tvScreen->fam_next_blue, SLOT(Text(QString)));

        //connect(cat,  SIGNAL(sigText(QString)), tvScreen->cat, SLOT(setText(QString)));

        connect(mainTimer, SIGNAL(sigTime(QString, QPalette)), tvScreen->sec, SLOT(showTime(QString, QPalette)));

        connect(mainTimer, SIGNAL(sigStarted(bool)), tvScreen->logo, SLOT(off_logo()));

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

    //connect(sec_doctor, SIGNAL(sigVisible(bool)), tvScreen->sec_doctor, SLOT(setVisible(bool)));
    connect(sec_doctor,  SIGNAL(sigTime(QString, QPalette)), tvScreen->sec_doctor, SLOT(showTime(QString, QPalette)));

    ui.leCam1->setObjectName("leCam1");
    ui.leCam2->setObjectName("leCam2");
    connect(ui.leCam1, SIGNAL(editingFinished()), this, SLOT(setCam()));
    connect(ui.leCam2, SIGNAL(editingFinished()), this, SLOT(setCam()));

//    QFile cam1("cam1.txt");
//    if(!cam1.exists()){
//        cam1.open(QIODevice::WriteOnly);
//        cam1.close();
//    }
//    else{
//        cam1.open(QIODevice::ReadOnly);
//        cam1Url = cam1.readAll();
//        cam1.close();
//    }

//    QFile cam2("cam2.txt");
//    if(!cam2.exists()){
//        cam2.open(QIODevice::WriteOnly);
//        cam2.close();
//    }
//    else{
//        cam2.open(QIODevice::ReadOnly);
//        cam2Url = cam2.readAll();
//        cam2.close();
//    }

    settings->beginGroup("URL");
    cam1Url = settings->value("cam1", "").toString();
    cam2Url = settings->value("cam2", "").toString();
    settings->endGroup();

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

    QDir dir("camera1");
    if(!dir.exists()){
        QDir dir;
        dir.mkdir("camera1");
    }

    dir.setPath("camera2");
    if(!dir.exists()){
        QDir dir;
        dir.mkdir("camera2");
    }

    QTimer* tmr = new QTimer(this);
    connect(tmr, SIGNAL(timeout()), this, SLOT(drawTvScreenshot()));
    tmr->start(100);

    QTimer* tmrCpu = new QTimer(this);
    connect(tmrCpu, SIGNAL(timeout()), this, SLOT(CpuUsage()));
    tmrCpu->start(1000);

//    QFile font_size("font_size.txt");
//    if(!font_size.exists()){
//        font_size.open(QIODevice::WriteOnly);
//        font_size.write("20");
//        font_size.close();
//    }
//    font_size.open(QIODevice::ReadOnly);
    //int size = font_size.readLine().toInt();

    settings->beginGroup("font");
    ui.cmbFont->setCurrentText(settings->value("font_size", "20").toString());
    settings->endGroup();

    //ui.cmbFont->setCurrentText(font_size.readLine());
    //QFont font_weight;
    //font_weight.setPixelSize(size);
    //tvScreen->cat->setFont(font_weight);\

    connect(mainTimer, SIGNAL(sigEndTime()), tvScreen->lblEndTimer,     SLOT(startProcess()));
    connect(mainTimer, SIGNAL(sigEndTime()), lblEndTimer,               SLOT(startProcess()));

    connect(sec_red_t,  SIGNAL(sigEndTime()), tvScreen->lblEndTimer,    SLOT(startProcess()));
    connect(sec_red_t,  SIGNAL(sigEndTime()), lblEndTimer,              SLOT(startProcess()));

    connect(sec_blue_t, SIGNAL(sigEndTime()), tvScreen->lblEndTimer,    SLOT(startProcess()));
    connect(sec_blue_t, SIGNAL(sigEndTime()), lblEndTimer,              SLOT(startProcess()));

    settings->beginGroup("timer");
    setSec(settings->value("height", 3).toInt());
    settings->endGroup();

    setSize();
    Variant(0);

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

    double fTotal;
    double fFree;
    QString memory;
    ULARGE_INTEGER free,total;
    bool bRes = ::GetDiskFreeSpaceExA( 0 , &free , &total , NULL );
    if ( bRes ){
        fFree = static_cast<__int64>(free.QuadPart);
        fTotal = static_cast<double>(static_cast<__int64>(total.QuadPart));
        memory = QString::number((fFree / fTotal) * 100).split(".").at(0);
    }else
        memory = "";

    mainwin->lblStatus->setText("Нагрузка ЦПУ: " + QString::number(t) +
                                " %\tСвободное место на диске: " + memory + " %");
}

void PCScreen::setCat(QString s){
    cat->setText(s);
    tvScreen->cat->setText(s);
}

void PCScreen::setAge(QString s){
    age->setText(s);
    tvScreen->age->setText(s);
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
            //QFile f("cam1.txt");
            //f.open(QIODevice::ReadOnly);
            //ui.leCam1->setText(f.readLine());
            settings->beginGroup("URL");
            ui.leCam1->setText(settings->value("cam1", "").toString());
            settings->endGroup();
            //f.close();
        }
        else{
            ui.cbAutoCam1->setEnabled(true);
            ui.leCam2->setStyleSheet("background-color: white");
            //QFile f("cam2.txt");
            //f.open(QIODevice::ReadOnly);
            //ui.leCam2->setText(f.readLine());
            settings->beginGroup("URL");
            ui.leCam2->setText(settings->value("cam2", "").toString());
            settings->endGroup();
            //f.close();
        }
        if(camConn){
            disconnect(camConn, SIGNAL(sigCamera(QString)), nullptr, nullptr);
            camConn->deleteLater();
        }
    }
}

void PCScreen::setCamera(QString ip){
    if(ui.cbAutoCam1->isChecked()){
        //QSettings settings("");
//        QFile f;
//        QTextStream out(&f);
//        f.setFileName("cam1.txt");
//        f.open(QIODevice::WriteOnly);

//        out << "srt://" + ip + ":1111";
//        f.close();
        cam1Url = "srt://" + ip + ":1111";
        settings->beginGroup("URL");
        settings->setValue("cam1", cam1Url);
        settings->endGroup();
        ui.cbAutoCam1->setChecked(false);
    }
    if(ui.cbAutoCam2->isChecked()){
//        QFile f;
//        QTextStream out(&f);
//        f.setFileName("cam2.txt");
//        f.open(QIODevice::WriteOnly);
//        out << "srt://" + ip + ":2222";
//        f.close();
        cam2Url = "srt://" + ip + ":2222";
        settings->beginGroup("URL");
        settings->setValue("cam2", cam2Url);
        settings->endGroup();
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
        //currentFileCam1 = "camera1/" + currentTime + ".mp4";
        //camera1->StartRecord(currentFileCam1);
        camera1->StartRecord("camera1/" + currentTime + ".mp4");
    }
    if(threadCam2->isRunning()){
        //currentFileCam2 = "camera2/" + currentTime + ".mp4";
        //camera2->StartRecord(currentFileCam2);
        camera2->StartRecord("camera2/" + currentTime + ".mp4");
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
    if(sender()->objectName() == "btnPlayLastWithSound1"){
        //l.append(currentFileCam1);
        QDir dir("camera1");
        QStringList dirList = dir.entryList(QDir::Files, QDir::Time);
        if(dirList.count() == 0)
            return;
        l.append("camera1/" + dirList.at(0));
        //qDebug()<<l;
    }else{
        //l.append(currentFileCam2);
        QDir dir("camera2");
        QStringList dirList = dir.entryList(QDir::Files, QDir::Time);
        if(dirList.count() == 0)
            return;
        l.append("camera2/" + dirList.at(0));

    }
    proc.start("ffplay", l);
    proc.waitForFinished(-1);
}

void PCScreen::PlaySlowMotion(){
    if(!slowMotionPlayer){
        if(sender()->objectName() == "btnPlayLastSlowMotion1"){
            //if(currentFileCam1 == "") return;
            //slowMotionPlayer = new PlayerViewer(currentFileCam1);
            QDir dir("camera1");
            QStringList dirList = dir.entryList(QDir::Files, QDir::Time);
            if(dirList.count() == 0)
                return;
            slowMotionPlayer = new PlayerViewer("camera1/" + dirList.at(0));
            connect(slowMotionPlayer, SIGNAL(sigClose()), this, SLOT(closePlayer()));
            btnPlayLastWithSound1->setEnabled(false);
            btnPlayLastSlowMotion1->setEnabled(false);
            btnPlayLastWithSound2->setEnabled(false);
            btnPlayLastSlowMotion2->setEnabled(false);
            btnPlaySlowMotion->setEnabled(false);
        }else{
            //if(currentFileCam2 == "") return;
            //slowMotionPlayer = new PlayerViewer(currentFileCam2);
            QDir dir("camera2");
            QStringList dirList = dir.entryList(QDir::Files, QDir::Time);
            if(dirList.count() == 0)
                return;
            slowMotionPlayer = new PlayerViewer("camera2/" + dirList.at(0));
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
//    QFile f;
    //QTextStream out(&f);
    if(sender()->objectName() == "leCam1"){
//        f.setFileName("cam1.txt");
//        f.open(QIODevice::WriteOnly);
//        out << ui.leCam1->text();
        cam1Url = ui.leCam1->text();
        settings->beginGroup("URL");
        settings->setValue("cam1", cam1Url);
        settings->endGroup();
    }else{
//        f.setFileName("cam2.txt");
//        f.open(QIODevice::WriteOnly);
//        out << ui.leCam2->text();
        settings->beginGroup("URL");
        settings->setValue("cam2", cam2Url);
        settings->endGroup();
        cam2Url = ui.leCam2->text();
    }
//    f.close();
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
//    qDebug()<<"close event";
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
//    qDebug()<<"close event";
    qApp->quit();

}

void PCScreen::setTime(){
    int min = uiTime.dMin->value();
    int sec1 = uiTime.dSec->value();
    int sec2 = uiTime.dSec2->value();
    if(min == 0 && sec1 == 0 && sec2 == 0)
        return;
    mainTimer->setTime(min * 60 + sec1 * 10 + sec2);
    //mainTimer->R
    //qDebug()<<"setTime";
}

void PCScreen::showView(){
    //QFile f1("cam1.txt");
    //f1.open(QIODevice::ReadOnly);

    settings->beginGroup("URL");
    ui.leCam1->setText(settings->value("cam1", "").toString());
    settings->endGroup();

    //ui.leCam1->setText(f1.readLine());
    //f1.close();
    //QFile f2("cam2.txt");
    //f2.open(QIODevice::ReadOnly);

    settings->beginGroup("URL");
    ui.leCam2->setText(settings->value("cam2", "").toString());
    settings->endGroup();

    //ui.leCam2->setText(f2.readLine());
    //f2.close();
    formView->show();
}

void PCScreen::paintEvent(QPaintEvent * ) {
	QPainter pn;
	pn.begin(this);
    //if (View == 0) {
		pn.setBrush(Qt::red);
		pn.drawRect(0, 0, width() / 2, height());
		pn.setBrush(Qt::blue);
		pn.drawRect(width() / 2, 0, width() / 2, height());
    //}
    //else {
        //pn.setBrush(Qt::black);
        //pn.drawRect(0, 0, width(), height());
    //}
	pn.end();
}

void PCScreen::keyPressEvent(QKeyEvent * pe){
    if(pe->key() == Qt::Key_F1){
        if(mainTimer->getStatus() != 1)
            frmTime->show();
    }
    else if(pe->key() == Qt::Key_F2){
        qDebug()<<"F2";
        choosingNames->showMaximized();
    }else
        emit sendKey(pe->key());
}

void PCScreen::setTimeFight(){
    if(mainTimer->getStatus() != 1)
        frmTime->show();
}

void PCScreen::closeTablo(){
    QKeyEvent *key_press = new QKeyEvent(QKeyEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier);
    QApplication::sendEvent(this, key_press);
}

void PCScreen::resetTablo(){
    //tvScreen->logo->on_logo();
    //tvScreen->grid->removeWidget(tvScreen->logo);
    QKeyEvent *key_press = new QKeyEvent(QKeyEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier);
    QApplication::sendEvent(this, key_press);
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
    age->setFont(f);

    lblEndTimer->setGeometry(0, 0, width(), height());

}

/*
void PCScreen::setFrameWidth(int f){
    mainTimer->setLineWidth(f);
    tvScreen->sec->setLineWidth(f);

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

	repaint();
}

void PCScreen::setSpace(int s){
    //grid->setSpacing(s);
    tvScreen->grid->setSpacing(s);
	repaint();
}
*/

void PCScreen::setSec(int m) {
    //qDebug()<<m;
	//grid->setMargin(m);
	//repaint();
    //grid->removeWidget(mainTimer);
    tvScreen->grid->removeWidget(tvScreen->sec);
    if(m == 3){
        //grid->addWidget(mainTimer, 24, 24, 13, 20);
        tvScreen->grid->addWidget(tvScreen->sec, 18, 25, 14, 18);
    }else if(m == 2){
        //grid->addWidget(mainTimer, 25, 24, 11, 20);
        tvScreen->grid->addWidget(tvScreen->sec, 19, 25, 12, 18);
    }else{
        //grid->addWidget(mainTimer, 26, 24, 9, 20);
        tvScreen->grid->addWidget(tvScreen->sec, 20, 25, 10, 18);
    }
    settings->beginGroup("timer");
    settings->setValue("height", m);
    settings->endGroup();
}

void PCScreen::setSize() {
    tvScreen->grid->setRowMinimumHeight(0, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
    tvScreen->grid->setRowMinimumHeight(1, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
    tvScreen->grid->setRowMinimumHeight(2, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
    tvScreen->grid->setRowMinimumHeight(3, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
    tvScreen->grid->setRowMinimumHeight(4, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
    tvScreen->grid->setRowMinimumHeight(5, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
    tvScreen->grid->setRowMinimumHeight(6, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
    tvScreen->grid->setRowMinimumHeight(7, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
    tvScreen->grid->setRowMinimumHeight(8, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
    tvScreen->grid->setRowMinimumHeight(9, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
    tvScreen->grid->setRowMinimumHeight(10, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);

    tvScreen->grid->setRowMinimumHeight(11, tvScreen->minimum_height + HEIGHT_REGION * tvScreen->percent_height / 5);
    tvScreen->grid->setRowMinimumHeight(12, tvScreen->minimum_height + HEIGHT_REGION * tvScreen->percent_height / 5);
    tvScreen->grid->setRowMinimumHeight(13, tvScreen->minimum_height + HEIGHT_REGION * tvScreen->percent_height / 5);
    tvScreen->grid->setRowMinimumHeight(14, tvScreen->minimum_height + HEIGHT_REGION * tvScreen->percent_height / 5);
    tvScreen->grid->setRowMinimumHeight(15, tvScreen->minimum_height + HEIGHT_REGION * tvScreen->percent_height / 5);

    tvScreen->grid->setRowMinimumHeight(41, tvScreen->minimum_height + HEIGHT_NEXT_FAMILY * tvScreen->percent_height / 4);
    tvScreen->grid->setRowMinimumHeight(42, tvScreen->minimum_height + HEIGHT_NEXT_FAMILY * tvScreen->percent_height / 4);
    tvScreen->grid->setRowMinimumHeight(43, tvScreen->minimum_height + HEIGHT_NEXT_FAMILY * tvScreen->percent_height / 4);
    tvScreen->grid->setRowMinimumHeight(44, tvScreen->minimum_height + HEIGHT_NEXT_FAMILY * tvScreen->percent_height / 4);
}

void PCScreen::changeSize() {
    //QSettings settings(fileSettings, QSettings::IniFormat);
    settings->beginGroup("height");

    HEIGHT_REGION = settings->value("HEIGHT_REGION", 0).toInt();
    HEIGHT_FAMILY = settings->value("HEIGHT_FAMILY", 0).toInt();
    HEIGHT_NEXT_FAMILY = settings->value("HEIGHT_NEXT_FAMILY", 0).toInt();


    int i = 0;
    if(sender()->objectName() == "btnNameDown")     i = 1;
    if(sender()->objectName() == "btnRegUp")        i = 2;
    if(sender()->objectName() == "btnRegDown")      i = 3;
    if(sender()->objectName() == "btnNextNameUp")   i = 4;
    if(sender()->objectName() == "btnNextNameDown") i = 5;

	if (i == 0) {
		if (HEIGHT_FAMILY < 5) {
			HEIGHT_FAMILY += 1;
            tvScreen->grid->setRowMinimumHeight(0, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
            tvScreen->grid->setRowMinimumHeight(1, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
            tvScreen->grid->setRowMinimumHeight(2, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
            tvScreen->grid->setRowMinimumHeight(3, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
            tvScreen->grid->setRowMinimumHeight(4, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
            tvScreen->grid->setRowMinimumHeight(5, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
            tvScreen->grid->setRowMinimumHeight(6, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
            tvScreen->grid->setRowMinimumHeight(7, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
            tvScreen->grid->setRowMinimumHeight(8, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
            tvScreen->grid->setRowMinimumHeight(9, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
            tvScreen->grid->setRowMinimumHeight(10, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
            settings->setValue("HEIGHT_FAMILY", HEIGHT_FAMILY);
		}
	}
	else if (i == 1) {
        if (HEIGHT_FAMILY > -10) {
			HEIGHT_FAMILY -= 1;
            if (HEIGHT_FAMILY != -10) {
                tvScreen->grid->setRowMinimumHeight(0, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
                tvScreen->grid->setRowMinimumHeight(1, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
                tvScreen->grid->setRowMinimumHeight(2, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
                tvScreen->grid->setRowMinimumHeight(3, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
                tvScreen->grid->setRowMinimumHeight(4, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
                tvScreen->grid->setRowMinimumHeight(5, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
                tvScreen->grid->setRowMinimumHeight(6, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
                tvScreen->grid->setRowMinimumHeight(7, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
                tvScreen->grid->setRowMinimumHeight(8, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
                tvScreen->grid->setRowMinimumHeight(9, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
                tvScreen->grid->setRowMinimumHeight(10, tvScreen->minimum_height + HEIGHT_FAMILY * tvScreen->percent_height / 6);
                settings->setValue("HEIGHT_FAMILY", HEIGHT_FAMILY);
			}
		}
	}
	else if (i == 2) {
		if (HEIGHT_REGION < 5) {
			HEIGHT_REGION += 1;
            tvScreen->grid->setRowMinimumHeight(11, tvScreen->minimum_height + HEIGHT_REGION * tvScreen->percent_height / 5);
            tvScreen->grid->setRowMinimumHeight(12, tvScreen->minimum_height + HEIGHT_REGION * tvScreen->percent_height / 5);
            tvScreen->grid->setRowMinimumHeight(13, tvScreen->minimum_height + HEIGHT_REGION * tvScreen->percent_height / 5);
            tvScreen->grid->setRowMinimumHeight(14, tvScreen->minimum_height + HEIGHT_REGION * tvScreen->percent_height / 5);
            tvScreen->grid->setRowMinimumHeight(15, tvScreen->minimum_height + HEIGHT_REGION * tvScreen->percent_height / 5);
            settings->setValue("HEIGHT_REGION", HEIGHT_REGION);
		}
	}
    else if (i == 3){
        if (HEIGHT_REGION > -8) {
			HEIGHT_REGION -= 1;
            if (HEIGHT_REGION != -8) {
                tvScreen->grid->setRowMinimumHeight(11, tvScreen->minimum_height + HEIGHT_REGION * tvScreen->percent_height / 5);
                tvScreen->grid->setRowMinimumHeight(12, tvScreen->minimum_height + HEIGHT_REGION * tvScreen->percent_height / 5);
                tvScreen->grid->setRowMinimumHeight(13, tvScreen->minimum_height + HEIGHT_REGION * tvScreen->percent_height / 5);
                tvScreen->grid->setRowMinimumHeight(14, tvScreen->minimum_height + HEIGHT_REGION * tvScreen->percent_height / 5);
                tvScreen->grid->setRowMinimumHeight(15, tvScreen->minimum_height + HEIGHT_REGION * tvScreen->percent_height / 5);
                settings->setValue("HEIGHT_REGION", HEIGHT_REGION);
			}
		}
	}
    else if (i == 4) {
        if (HEIGHT_NEXT_FAMILY < 5) {
            HEIGHT_NEXT_FAMILY += 1;
            tvScreen->grid->setRowMinimumHeight(41, tvScreen->minimum_height + HEIGHT_NEXT_FAMILY * tvScreen->percent_height / 4);
            tvScreen->grid->setRowMinimumHeight(42, tvScreen->minimum_height + HEIGHT_NEXT_FAMILY * tvScreen->percent_height / 4);
            tvScreen->grid->setRowMinimumHeight(43, tvScreen->minimum_height + HEIGHT_NEXT_FAMILY * tvScreen->percent_height / 4);
            tvScreen->grid->setRowMinimumHeight(44, tvScreen->minimum_height + HEIGHT_NEXT_FAMILY * tvScreen->percent_height / 4);
            settings->setValue("HEIGHT_NEXT_FAMILY", HEIGHT_NEXT_FAMILY);
        }
    }
    else {
        if (HEIGHT_NEXT_FAMILY > -6) {
            HEIGHT_NEXT_FAMILY -= 1;
            if (HEIGHT_NEXT_FAMILY != -6) {
                tvScreen->grid->setRowMinimumHeight(41, tvScreen->minimum_height + HEIGHT_NEXT_FAMILY * tvScreen->percent_height / 4);
                tvScreen->grid->setRowMinimumHeight(42, tvScreen->minimum_height + HEIGHT_NEXT_FAMILY * tvScreen->percent_height / 4);
                tvScreen->grid->setRowMinimumHeight(43, tvScreen->minimum_height + HEIGHT_NEXT_FAMILY * tvScreen->percent_height / 4);
                tvScreen->grid->setRowMinimumHeight(44, tvScreen->minimum_height + HEIGHT_NEXT_FAMILY * tvScreen->percent_height / 4);
                settings->setValue("HEIGHT_NEXT_FAMILY", HEIGHT_NEXT_FAMILY);
            }
        }
    }
    settings->endGroup();
    qDebug()<<"i = "<<i;
}

void PCScreen::drawTvScreenshot(){
    if(formView->isVisible()){
        QPixmap pix = tvScreen->grab();
        pix = pix.scaled(ui.lblTvscreen->width(), ui.lblTvscreen->height());
        ui.lblTvscreen->setPixmap(pix);
    }
}

void PCScreen::HIDE(QString s1, QString s2, QString s3, QString s4){
    emit sig_hide(s1, s2, s3, s4);
}

void PCScreen::newListSportsmens(){
    lf->deleteLater();
    lf = new ListFamily(this);
    connect(lf->weight, SIGNAL(currentTextChanged(QString)), this, SLOT(setCat(QString)));
    connect(lf->age, SIGNAL(currentTextChanged(QString)), this, SLOT(setAge(QString)));
    connect(lf, SIGNAL(sig_hide(QString, QString, QString, QString)), this, SLOT(HIDE(QString, QString, QString, QString)));
}

void PCScreen::showListSportsmens(){
    //ListFamily* lf = new ListFamily;
    //connect(lf->weight, SIGNAL(currentTextChanged(QString)), this, SLOT(setCat(QString)));
    //connect(lf->age, SIGNAL(currentTextChanged(QString)), this, SLOT(setAge(QString)));
    //connect(lf, SIGNAL(sig_hide(QString, QString, QString, QString)), this, SLOT(HIDE(QString, QString, QString, QString)));
    lf->showFullScreen();
}

void PCScreen::changeFontWeight(QString s){
    QFile font_size("font_size.txt");

    //font_size.open(QIODevice::WriteOnly);
    //QTextStream out(&font_size);

    settings->beginGroup("font");
    settings->setValue("font_size", s);
    settings->endGroup();

    //out << s;
    //font_size.close();



    QFont font;
    font.setPixelSize(s.toInt());
    tvScreen->cat->setFont(font);
    tvScreen->age->setFont(font);
}

void PCScreen::turnDoctor(){
    if(sec_doctor->isVisible()){
        sec_doctor->setVisible(false);
        sec_doctor->StartStop();
        sec_doctor->Reset();
        tvScreen->sec_doctor->setVisible(false);
    }else{
        sec_doctor->setVisible(true);
        sec_doctor->StartStop();
        tvScreen->sec_doctor->setVisible(true);
    }
}


void PCScreen::addDisplay(int i){
    if(i == 0){
        udpTimer->stop();
        flagUdp = 0;
        return;
    }
    QByteArray baDatagram;
    QDataStream out(&baDatagram, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);
    //QDateTime dt = QDateTime::currentDateTime();
    baDatagram.append("hello display!");
    //out << baDatagram;
    QHostAddress addr;
    addr.setAddress("192.168.0.255");
    int count = s_udp->writeDatagram(baDatagram, addr, 2424);
    QWidget* w = new QWidget;
    w->setWindowTitle("Выбор IP адреса");
    w->setWindowFlag(Qt::WindowStaysOnTopHint);
    w->setMinimumWidth(300);

    int y = 10;

    foreach (const QNetworkInterface &netInterface, QNetworkInterface::allInterfaces()) {
        QNetworkInterface::InterfaceFlags flags = netInterface.flags();

        if( (bool)(flags & QNetworkInterface::IsRunning) && !(bool)(flags & QNetworkInterface::IsLoopBack)){
            foreach (const QNetworkAddressEntry &address, netInterface.addressEntries()) {
                if(address.ip().protocol() == QAbstractSocket::IPv4Protocol){

                    qDebug() << address.ip().toString()<<y;
                    QRadioButton* rb = new QRadioButton(address.ip().toString(), w);
                    rb->move(10, y);
                    connect(rb, SIGNAL(toggled(bool)), this, SLOT(setAddress(bool)));
                    y += 20;

                }
            }
        }
    }

    w->show();
    qDebug()<<count;
}

void PCScreen::setAddress(bool state){

    if(state){
        address = static_cast<QRadioButton*>(sender())->text();
        QList<QString> l_address = address.split(".");
        address = l_address.at(0) + "." + l_address.at(1) + "." +l_address.at(2) + ".255";
        //start timer
        qDebug()<<address;
        flagUdp = 0;
        udpTimer->start(1000);
    }

}

void PCScreen::udpSend(){
    qDebug()<<"udpSend";
    QByteArray baDatagram;
    QDataStream out(&baDatagram, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3);
    if(flagUdp == 0){
        baDatagram.append("hello display!");
        QHostAddress addr;
        addr.setAddress(address);
        s_udp->writeDatagram(baDatagram, addr, 2424);
    }
    if(flagUdp == 1){
        QString data;
        data.append(rateRed->text() + ";" + rateBlue->text() + ";");
        data.append(np_red->text() + ";" + np_blue->text() + ";");

        data.append(mainTimer->getTime() + ";");
        if(mainTimer->getStatus() == 1)
            data.append("1;");
        else
            data.append("0;");

        data.append(sec_red->getTime() + ";");
        if(sec_red->isVisible()){
            if(sec_red->getStatus() == 1)
                data.append("1;");
            else
                data.append("0;");
        }else
            data.append("-1;");

        data.append(sec_blue->getTime() + ";");
        if(sec_blue->isVisible()){
            if(sec_blue->getStatus() == 1)
                data.append("1;");
            else
                data.append("0;");
        }else
            data.append("-1;");

        if(plus_red->getText() == "+")
            data.append("1");
        else if(plus_blue->getText() == "+")
            data.append("2");
        else
            data.append("0");

        baDatagram.append(data);
        //QHostAddress addr;
        //addr.setAddress(address);
        s_udp->writeDatagram(baDatagram, *remoteAddress, 2424);
    }
}

void PCScreen::slotProcessDatagrams(){
    QByteArray baDatagram;
    QHostAddress addr;
    do{
        baDatagram.resize(s_udp->pendingDatagramSize());
        s_udp->readDatagram(baDatagram.data(), baDatagram.size(), &addr);
    }while(s_udp->hasPendingDatagrams());
    qDebug()<<"QString(baDatagram) = "<<QString(baDatagram);
    if(QString(baDatagram) == "ok tablo!"){
        flagUdp = 1;
        udpTimer->start(500);
        *remoteAddress = addr;
        qDebug()<<"udpTimer->stop()"<<addr.toString();
    }
}

void PCScreen::sbrosLogo(){
    tvScreen->logo->on_logo();
}

void PCScreen::Variant(int variant){
    if(variant == 0){
        viewCam1->setVisible(false);
        btnPlayLastWithSound1->setVisible(false);
        btnPlayLastSlowMotion1->setVisible(false);
        btnPlaySlowMotion->setVisible(false);
        cbCam1->setVisible(false);
        viewCam2->setVisible(false);
        btnPlayLastWithSound2->setVisible(false);
        btnPlayLastSlowMotion2->setVisible(false);
        btnStopRecord->setVisible(false);
        cbCam2->setVisible(false);
        lbl->setVisible(false);
        cbAddDisp->setVisible(false);
    }
    else{
        viewCam1->setVisible(true);
        btnPlayLastWithSound1->setVisible(true);
        btnPlayLastSlowMotion1->setVisible(true);
        btnPlaySlowMotion->setVisible(true);
        cbCam1->setVisible(true);
        viewCam2->setVisible(true);
        btnPlayLastWithSound2->setVisible(true);
        btnPlayLastSlowMotion2->setVisible(true);
        btnStopRecord->setVisible(true);
        cbCam2->setVisible(true);
        lbl->setVisible(true);
        cbAddDisp->setVisible(true);
    }
}

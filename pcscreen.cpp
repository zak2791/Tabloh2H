 #include <QPainter>
#include <QGridLayout>
#include <QPushButton>
#include <QEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QDebug>
#include <QGuiApplication>
#include <QScreen>
#include <QMessageBox>
#include <QPalette>

#include <QFileDialog>
#include <QTime>
#include "pcscreen.h"
#include "QAction"
#include <math.h>
#include <QHttpServer>

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

    mainwin = mw;

    address = "";
    remoteAddress = new QHostAddress;
    s_udp = new QUdpSocket(this);
    s_udp->bind(2425 );
    flagUdp = 0;
    connect(s_udp, SIGNAL(readyRead()), this, SLOT(slotProcessDatagrams()));
    udpTimer = new QTimer(this);
    connect(udpTimer, SIGNAL(timeout()), this, SLOT(udpSend()));

	col_red = "white";
	col_blue = "white";

#ifdef APP_LAUNCH_FROM_IDE
    fileSettings = "settings.ini";
    QString fProc = "server.exe";
#else
    fileSettings = "bin/settings.ini";
    QString fProc = "bin/server.exe";
#endif

    //myProcess = new QProcess(parent);
    //myProcess->start(fProc);

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

    IconButton* doctor = new IconButton(":/images/doctor.svg");
    doctor->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    connect(doctor, SIGNAL(clicked()), this, SLOT(turnDoctor()));

    IconButton * btnParter_red = new IconButton(":/images/parter_red.svg");
	btnParter_red->setObjectName("btnParter_red");
	btnParter_red->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	
    IconButton * btnTime = new IconButton(":/images/time.svg");
    btnTime->setObjectName("btnTime");
	btnTime->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    IconButton * btnParter_blue = new IconButton(":/images/parter_blue.svg");
	btnParter_blue->setObjectName("btnParter_blue");
	btnParter_blue->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    IconButton * btnTehTime_red = new IconButton(":/images/ttech_red.svg");
	btnTehTime_red->setObjectName("btnTehTime_red");
	btnTehTime_red->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	btnTehTime_red->setStyleSheet("color: red; font: bold " + QString::number(round(btnTehTime_red->height() / 2)) + "px;");

    IconButton * btnSettings = new IconButton(":/images/sportsmens.svg");
	btnSettings->setObjectName("btnSettings");
	btnSettings->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    connect(btnSettings, SIGNAL(clicked()), this, SLOT(showListSportsmens()));
	
    IconButton * btnTehTime_blue = new IconButton(":/images/ttech_blue.svg");
	btnTehTime_blue->setObjectName("btnTehTime_blue");
	btnTehTime_blue->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    IconButton * btnPlus_red = new IconButton(":/images/plus_red.svg");
	btnPlus_red->setObjectName("btnPlus_red");
	btnPlus_red->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	btnPlus_red->setStyleSheet("color: red; font: bold " + QString::number(btnPlus_red->height()) + "px;");

    IconButton * btnPlus_blue = new IconButton(":/images/plus_blue.svg");
	btnPlus_blue->setObjectName("btnPlus_blue");
	btnPlus_blue->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    btnPlus_blue->setStyleSheet("color: blue; font: bold " + QString::number(btnPlus_blue->height()) + "px;");

    plus_red = new Plus(col_red, this);
	plus_red->setObjectName("plus_red");
    plus_red->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    plus_blue = new Plus(col_blue, this);
	plus_blue->setObjectName("plus_blue");
    plus_blue->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    mainTimer = new LCDTimer(this);
    mainTimer->setObjectName("mainTimer");

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

    cat = new QPushButton(this);//("yellow", this);
    cat->setAutoFillBackground(true);
    cat->setStyleSheet("background-color: black; color: white; text-align: center");
    cat->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    age = new QPushButton(this);//("yellow", this);
    age->setAutoFillBackground(true);
    age->setStyleSheet("background-color: black; color: white; text-align: center");
    age->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    initListNames();

//    //
//    choosingNames = new ChoosingNames;

//    lf = new ListFamily(this);
//    lf->setObjectName("lf");

//    choosingNames->setNames(lf->getSportsmens());
//    choosingNames->setAge(lf->lAge);
//    choosingNames->setWeight(lf->lWeight);

//    connect(choosingNames, SIGNAL(close(QString, QString, QString, QString, QString, QString, QString, QString)),
//                this, SLOT(closeWinName(QString, QString, QString, QString, QString, QString, QString, QString)));
//    //

	formView = new QWidget;
    ui.setupUi(formView);
    WidgetFilter* wf = new WidgetFilter(formView);
    formView->installEventFilter(wf);
    connect(wf, SIGNAL(sigClose()), this, SLOT(closeView()));

    connect(ui.btnNameDown,     SIGNAL(clicked()),      this, SLOT(changeSize()));
    connect(ui.btnNameUp,       SIGNAL(clicked()),      this, SLOT(changeSize()));
    connect(ui.btnNextNameDown, SIGNAL(clicked()),      this, SLOT(changeSize()));
    connect(ui.btnNextNameUp,   SIGNAL(clicked()),      this, SLOT(changeSize()));
    connect(ui.btnRegDown,      SIGNAL(clicked()),      this, SLOT(changeSize()));
    connect(ui.btnRegUp,        SIGNAL(clicked()),      this, SLOT(changeSize()));

    //connect(ui.cbFullScreen,    SIGNAL(toggled(bool)),  this, SLOT(tvFullScreen(bool)));

    ui.cmbFont->addItems({"10", "12", "14", "16", "18", "20", "23", "26", "29", "34", "39", "45", "50", "55", "60",
                          "65", "70", "75", "80", "85", "90", "100", "110", "120", "130", "140", "150", "170", "180", "190", "200", "210", "220", "230"});
    connect(ui.cmbFont,         SIGNAL(currentTextChanged(QString)), this, SLOT(changeFontWeight(QString)));

    frmTime = new QWidget;
    uiTime.setupUi(frmTime);

    connect(mainwin->winSettings, SIGNAL(triggered()), this, SLOT(showView()));

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

    QLabel* lbl2 = new QLabel("Установка времени боя - 'F1', сброс - 'Backspace', выход - 'Esc'");
    lbl2->setAlignment(Qt::AlignCenter);
    lbl2->setStyleSheet("color: white; font-size: 12pt");

    QLabel* lbl3 = new QLabel("Добавить спортсмена - 'F2'");
    //lbl3->setAlignment(Qt::);
    lbl3->setStyleSheet("color: white; font-size: 12pt");

    connect(mainwin->time, SIGNAL(triggered()), this, SLOT(setTimeFight()));

    connect(mainwin->closeProg, SIGNAL(triggered()), this, SLOT(closeTablo()));

    connect(mainwin->actSbros, SIGNAL(triggered()), this, SLOT(resetTablo()));

	grid = new QGridLayout(this);
	grid->setObjectName("grid");
	//spacing = 6;
	//margin = 6;
    //сетка 68х42
    grid->setSpacing(6);
    //grid->setMargin(6);
    grid->addWidget(fam_red,                0,  0,  4,  34);
    grid->addWidget(fam_blue,               0,  34, 4,  34);
    grid->addWidget(reg_red,                4, 0,  4,  34);
    grid->addWidget(reg_blue,               4, 34, 4,  34);

    grid->addWidget(rateRed,                8,  0,  20, 24);
    grid->addWidget(rateBlue,               8,  44, 20, 24);

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

    grid->addWidget(btnParter_red,          26, 24, 2,  6);
    grid->addWidget(btnTime,                26, 31, 2,  6);
    grid->addWidget(btnParter_blue,         26, 38, 2,  6);

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

    btnTime->setStyleSheet("color: green; font: bold " + QString::number(round(btnTime->height() / 2)) + "px;");

    lblEndTimer = new EndTime(this);
    lblEndTimer->setObjectName("lblEndTimer");

    if(QGuiApplication::screens().count() == 1){
        QMessageBox::information(this, "ВНИМАНИЕ!",
        "Подключите к компьютеру дисплей в режиме \"Расширенный рабочий стол!\"",
		QMessageBox::Ok);
	}

        //frmTvSettings = new QWidget;
        //uiTV.setupUi(frmTvSettings);
        //connect(mainwin->tvSettings, SIGNAL(triggered()), frmTvSettings, SLOT(show()));
        //connect(uiTV.sbX, SIGNAL(valueChanged(int)), this, SLOT(tvXchange(int)));
        //connect(uiTV.sbY, SIGNAL(valueChanged(int)), this, SLOT(tvYchange(int)));
        //connect(uiTV.sbW, SIGNAL(valueChanged(int)), this, SLOT(tvWchange(int)));
        //connect(uiTV.sbH, SIGNAL(valueChanged(int)), this, SLOT(tvHchange(int)));
        //connect(uiTV.btnReset, SIGNAL(clicked()), this, SLOT(tvReset()));

        tvScreen = new TVScreen;

        setTvScreenGeometry();

        tvScreen->show();

        connect(rateRed,	SIGNAL(sigRate(int)),		 tvScreen->ball_red,	  SLOT(setRate(int)));
        connect(rateBlue,	SIGNAL(sigRate(int)),		 tvScreen->ball_blue,     SLOT(setRate(int)));

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

        connect(fam_next_blue, SIGNAL(sigText(QString)), tvScreen->fam_next_blue, SLOT(Text(QString)));

        connect(mainTimer, SIGNAL(sigTime(QString, QPalette)), tvScreen->sec, SLOT(showTime(QString, QPalette)));
        connect(mainTimer, SIGNAL(sigIntTime(int)), this, SLOT(saveTime(int)));


        connect(mainTimer, SIGNAL(sigStarted(bool)), tvScreen->logo, SLOT(off_logo()));

        connect(uiTime.dMin, SIGNAL(valueChanged(int)), this, SLOT(setTime()));
        connect(uiTime.dSec, SIGNAL(valueChanged(int)), this, SLOT(setTime()));
        connect(uiTime.dSec2, SIGNAL(valueChanged(int)), this, SLOT(setTime()));

    connect(sec_red, SIGNAL(sigVisible(bool)), tvScreen->sec_red, SLOT(setVisible(bool)));
    connect(sec_red,  SIGNAL(sigTime(QString, QPalette)), tvScreen->sec_red, SLOT(showTime(QString, QPalette)));

    connect(sec_blue, SIGNAL(sigVisible(bool)), tvScreen->sec_blue, SLOT(setVisible(bool)));
    connect(sec_blue,  SIGNAL(sigTime(QString, QPalette)), tvScreen->sec_blue, SLOT(showTime(QString, QPalette)));

    connect(sec_red_t, SIGNAL(sigVisible(bool)), tvScreen->sec_red_t, SLOT(setVisible(bool)));
    connect(sec_red_t,  SIGNAL(sigTime(QString, QPalette)), tvScreen->sec_red_t, SLOT(showTime(QString, QPalette)));

    connect(sec_blue_t, SIGNAL(sigVisible(bool)), tvScreen->sec_blue_t, SLOT(setVisible(bool)));
    connect(sec_blue_t,  SIGNAL(sigTime(QString, QPalette)), tvScreen->sec_blue_t, SLOT(showTime(QString, QPalette)));

    connect(sec_doctor,  SIGNAL(sigTime(QString, QPalette)), tvScreen->sec_doctor, SLOT(showTime(QString, QPalette)));

    ui.leCam1->setObjectName("leCam1");
    ui.leCam2->setObjectName("leCam2");
    connect(ui.leCam1, SIGNAL(editingFinished()), this, SLOT(setCam()));
    connect(ui.leCam2, SIGNAL(editingFinished()), this, SLOT(setCam()));

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

    settings->beginGroup("font");
    ui.cmbFont->setCurrentText(settings->value("font_size", "20").toString());
    settings->endGroup();

    connect(mainTimer, SIGNAL(sigEndTime()), tvScreen->lblEndTimer,     SLOT(startProcess()));
    connect(mainTimer, SIGNAL(sigEndTime()), lblEndTimer,               SLOT(startProcess()));

    connect(sec_red_t,  SIGNAL(sigEndTime()), tvScreen->lblEndTimer,    SLOT(startProcess()));
    connect(sec_red_t,  SIGNAL(sigEndTime()), lblEndTimer,              SLOT(startProcess()));

    connect(sec_blue_t, SIGNAL(sigEndTime()), tvScreen->lblEndTimer,    SLOT(startProcess()));
    connect(sec_blue_t, SIGNAL(sigEndTime()), lblEndTimer,              SLOT(startProcess()));

    settings->beginGroup("timer");
    setSec(settings->value("height", 3).toInt());
    settings->endGroup();

    settings->beginGroup("time");
    int iTime = settings->value("initTime", 180).toInt();
    int cTime = settings->value("current time", 180).toInt();
    qDebug()<<iTime<<cTime;
    mainTimer->setTime(cTime, iTime);
    settings->endGroup();

    settings->beginGroup("rates");
    int rRed = settings->value("rateRed", 0).toInt();
    int rBlue = settings->value("rateBlue", 0).toInt();
    //qDebug()<<iTime<<cTime;
    rateRed->setRate(rRed);
    rateBlue->setRate(rBlue);
    QString npR = settings->value("npRed", "").toString();
    QString npB = settings->value("npBlue", "").toString();
    np_red->setValue(npR);
    np_blue->setValue(npB);
    QString plus = settings->value("plus", "").toString();
    if(plus == "red")
        plus_red->setData("+");
    if(plus == "blue")
        plus_blue->setData("+");
    settings->endGroup();

    settings->beginGroup("names");
    QString nRed =      settings->value("nameRed", "").toString();
    QString nBlue =     settings->value("nameBlue", "").toString();
    QString nNextRed =  settings->value("nameNextRed", "").toString();
    QString nNextBlue = settings->value("nameNextBlue", "").toString();
    QString regRed =    settings->value("regionRed", "").toString();
    QString regBlue =   settings->value("regionBlue", "").toString();
    fam_red->Text(nRed);
    fam_blue->Text(nBlue);
    fam_next_red->Text(nNextRed);
    fam_next_blue->Text(nNextBlue);
    reg_red->Text(regRed);
    reg_blue->Text(regBlue);
    settings->endGroup();

    connect(rateRed,	SIGNAL(sigRate(int)), this,	  SLOT(saveConditionRate(int)));
    connect(rateBlue,	SIGNAL(sigRate(int)), this,	  SLOT(saveConditionRate(int)));

    connect(np_red,	 SIGNAL(ball(QString)), this,  SLOT(saveConditionRules(QString)));
    connect(np_blue, SIGNAL(ball(QString)), this,  SLOT(saveConditionRules(QString)));

    connect(fam_red,       SIGNAL(sigText(QString)), this, SLOT(saveConditionNames(QString)));
    connect(fam_blue,      SIGNAL(sigText(QString)), this, SLOT(saveConditionNames(QString)));
    connect(fam_next_red,  SIGNAL(sigText(QString)), this, SLOT(saveConditionNames(QString)));
    connect(fam_next_blue, SIGNAL(sigText(QString)), this, SLOT(saveConditionNames(QString)));
    connect(reg_red,       SIGNAL(sigText(QString)), this, SLOT(saveConditionNames(QString)));
    connect(reg_blue,      SIGNAL(sigText(QString)), this, SLOT(saveConditionNames(QString)));

    connect(plus_red,  SIGNAL(textChange(QString)), this, SLOT(saveConditionPlus(QString)));
    connect(plus_blue, SIGNAL(textChange(QString)), this, SLOT(saveConditionPlus(QString)));

    connect(ui.cbShowOnTv, SIGNAL(toggled(bool)), tvScreen, SLOT(setPlayerEnabled(bool)));

    setSize();
    Variant(0);

    connect(this, SIGNAL(sigLogo(bool)), tvScreen, SIGNAL(sigLogo(bool)));

    // socketDataToVideo = new QUdpSocket;
    // datagram = new QNetworkDatagram;
    // datagram->setDestination(QHostAddress::LocalHost, 5555);

    html =  "<!DOCTYPE html>"
                "<html>"
                    "<head>"
                        "<meta http-equiv='Refresh' content='1' charset='utf-8'/>"
                        "<style>"
                            "td {font-family: 'Times New Roman', Georgia, Serif; text-align: center;}"
                            ".red {color: white; background-color: rgba(255,0,0,0.6);}"
                            ".name {width: 30%;}"
                            ".blue {color: white; background-color: rgba(0,0,255,0.6);}"
                            ".time {color: white; background-color: rgba(0,255,0,0.6); font-size: 30px;}"
                            ".rate {font-size: 30px; width: 10%;}"
                        "</style>"
                    "</head>"
                    "<table width='1000px'>"
                        "<tr>"
                            "<td class='red name'> %1 </td>"
                            "<td rowspan='2' class='rate red'> %2 </td>"
                            "<td rowspan='2' class='time'> %3 </td>"
                            "<td rowspan='2' class='blue rate'> %4 </td>"
                            "<td class='blue name'> %5 </td>"
                        "</tr>"
                        "<tr>"
                            "<td class='red'> %6 </td>"
                            "<td class='blue'> %7 </td>"
                        "</tr>"
                    "</table>"
                "</html>";
    sHtml = html.arg("", "", "", "", "", "", "");
    QHttpServer* server = new QHttpServer;
    server->listen(QHostAddress::Any, 3333);
    server->route("/", [this]() {
        return sHtml.toUtf8();
    });

    connect(ui.cbTurnVK, SIGNAL(toggled(bool)), this, SLOT(turnTranslationToVk(bool)));
}

PCScreen::~PCScreen()
{
    //qDebug()<<"kill2";
    // myProcess->kill();
    // qDebug()<<"kill";
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

void PCScreen::initListNames()
{

    choosingNames = new ChoosingNames;
    lf = new ListFamily(this);
    lf->setObjectName("lf");
    choosingNames->setNames(lf->getSportsmens());
    choosingNames->setAge(lf->lAge);
    choosingNames->setWeight(lf->lWeight);

    connect(choosingNames, SIGNAL(close(QString, QString, QString, QString, QString, QString, QString, QString)),
                this, SLOT(closeWinName(QString, QString, QString, QString, QString, QString, QString, QString)));
    connect(choosingNames, SIGNAL(del()), this, SLOT(delListNames()));
}

// void PCScreen::slotExit()
// {
//     //myProcess->kill();
//     //qDebug()<<"kill";
// }

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
            settings->beginGroup("URL");
            ui.leCam1->setText(settings->value("cam1", "").toString());
            settings->endGroup();
            //f.close();
        }
        else{
            ui.cbAutoCam1->setEnabled(true);
            ui.leCam2->setStyleSheet("background-color: white");
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
        cam1Url = "srt://" + ip + ":1111";
        settings->beginGroup("URL");
        settings->setValue("cam1", cam1Url);
        settings->endGroup();
        ui.cbAutoCam1->setChecked(false);
    }
    if(ui.cbAutoCam2->isChecked()){
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
        camera1->StartRecord("camera1/" + currentTime + ".mp4");
    }
    if(threadCam2->isRunning()){
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
    camera1->StopRecord();
    camera2->StopRecord();
    btnPlayLastWithSound1->setEnabled(true);
    btnPlayLastSlowMotion1->setEnabled(true);
    btnPlayLastWithSound2->setEnabled(true);
    btnPlayLastSlowMotion2->setEnabled(true);
    btnPlaySlowMotion->setEnabled(true);
    btnStopRecord->setEnabled(false);
}

void PCScreen::PlayFile(){
    QProcess proc;
    QStringList l;
    l.append(QString("-fs"));
    if(sender()->objectName() == "btnPlayLastWithSound1"){
        QDir dir("camera1");
        QStringList dirList = dir.entryList(QDir::Files, QDir::Time);
        if(dirList.count() == 0)
            return;
        l.append("camera1/" + dirList.at(0));
    }else{
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
            QDir dir("camera1");
            QStringList dirList = dir.entryList(QDir::Files, QDir::Time);
            if(dirList.count() == 0)
                return;
            slowMotionPlayer = new PlayerViewer("camera1/" + dirList.at(0));
            connect(slowMotionPlayer, SIGNAL(sigClose()), this, SLOT(closePlayer()));
            connect(slowMotionPlayer, SIGNAL(sigClose()), tvScreen, SLOT(hidePlayer()));
            connect(slowMotionPlayer, SIGNAL(sigImage(QImage)), tvScreen->player, SLOT(draw_image(QImage)));
            tvScreen->showPlayer();
            btnPlayLastWithSound1->setEnabled(false);
            btnPlayLastSlowMotion1->setEnabled(false);
            btnPlayLastWithSound2->setEnabled(false);
            btnPlayLastSlowMotion2->setEnabled(false);
            btnPlaySlowMotion->setEnabled(false);
        }else{
            QDir dir("camera2");
            QStringList dirList = dir.entryList(QDir::Files, QDir::Time);
            if(dirList.count() == 0)
                return;
            slowMotionPlayer = new PlayerViewer("camera2/" + dirList.at(0));
            connect(slowMotionPlayer, SIGNAL(sigClose()), this, SLOT(closePlayer()));
            connect(slowMotionPlayer, SIGNAL(sigClose()), tvScreen, SLOT(hidePlayer()));
            connect(slowMotionPlayer, SIGNAL(sigImage(QImage)), tvScreen->player, SLOT(draw_image(QImage)));
            tvScreen->showPlayer();
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
    connect(slowMotionPlayer, SIGNAL(sigClose()), tvScreen, SLOT(hidePlayer()));
    connect(slowMotionPlayer, SIGNAL(sigImage(QImage)), tvScreen->player, SLOT(draw_image(QImage)));
    tvScreen->showPlayer();
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
    if(sender()->objectName() == "leCam1"){
        cam1Url = ui.leCam1->text();
        settings->beginGroup("URL");
        settings->setValue("cam1", cam1Url);
        settings->endGroup();
    }else{
        settings->beginGroup("URL");
        settings->setValue("cam2", cam2Url);
        settings->endGroup();
        cam2Url = ui.leCam2->text();
    }
}

void PCScreen::turnCamera(bool state){
    if(state){
        if(sender()->objectName() == "cbCam1"){
            if(ui.cbTurnVK->isChecked())
                camera1->setUrl("udp://127.0.0.1:3333");
            else
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

void PCScreen::setTime(){
    int min = uiTime.dMin->value();
    int sec1 = uiTime.dSec->value();
    int sec2 = uiTime.dSec2->value();
    if(min == 0 && sec1 == 0 && sec2 == 0)
        return;
    int iTime = min * 60 + sec1 * 10 + sec2;
    if(mainTimer->isInitTime()){
        settings->beginGroup("time");
        settings->setValue("initTime", iTime);
        settings->endGroup();
    }
    mainTimer->setTime(iTime);

}

void PCScreen::showView(){
    settings->beginGroup("URL");
    ui.leCam1->setText(settings->value("cam1", "").toString());
    settings->endGroup();

    settings->beginGroup("URL");
    ui.leCam2->setText(settings->value("cam2", "").toString());
    settings->endGroup();

    formView->show();
}

void PCScreen::paintEvent(QPaintEvent * ) {
	QPainter pn;
	pn.begin(this);
		pn.setBrush(Qt::red);
		pn.drawRect(0, 0, width() / 2, height());
		pn.setBrush(Qt::blue);
		pn.drawRect(width() / 2, 0, width() / 2, height());
	pn.end();
}

void PCScreen::keyPressEvent(QKeyEvent * pe){
    if(pe->key() == Qt::Key_F1){
        if(mainTimer->getStatus() != 1)
            frmTime->show();
    }
    else if(pe->key() == Qt::Key_F2){
        //qDebug()<<"F2";
        choosingNames->showMaximized();
    }else
        emit sendKey(pe->key());
}

void PCScreen::setTimeFight(){
    if(mainTimer->getStatus() != 1)
        frmTime->show();
}

void PCScreen::closeTablo(){
    //myProcess->kill();
    qDebug()<<"closeTablo";
    QKeyEvent *key_press = new QKeyEvent(QKeyEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier);
    QApplication::sendEvent(this, key_press);
}

void PCScreen::resetTablo(){
    QKeyEvent *key_press = new QKeyEvent(QKeyEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier);
    QApplication::sendEvent(this, key_press);
}

void PCScreen::closeWinName(QString redName, QString redRegion, QString blueName,
                            QString blueRegion, QString redNameNext, QString blueNameNext,
                            QString Age, QString Weight){
    fam_red-> Text(redName);
    reg_red->Text(redRegion);
    fam_blue->Text(blueName);
    reg_blue->Text(blueRegion);
    fam_next_red->Text(redNameNext);
    fam_next_blue->Text(blueNameNext);
    age->setText(Age);
    cat->setText(Weight);
    tvScreen->age->setText(Age);
    tvScreen->cat->setText(Weight);
}

void PCScreen::delListNames()
{
    if(choosingNames != nullptr)
        choosingNames->deleteLater();
    if(lf != nullptr)
        lf->deleteLater();
    initListNames();
    choosingNames->showMaximized();
}

void PCScreen::saveTime(int iTime)
{
    settings->beginGroup("time");
    settings->setValue("current time", iTime);
    settings->endGroup();
    int minutes = iTime / 60;
    int secondes = iTime - minutes * 60;
    QString sSeconds;
    if(secondes < 10) sSeconds = "0" + QString::number(secondes);
    else sSeconds = QString::number(secondes);
    QString sTime = QString::number(minutes) + ":" + sSeconds;
    sHtml = html.arg(fam_red->getText(),
                             rateRed->text(),
                             sTime,
                             rateBlue->text(),
                             fam_blue->getText(),
                             reg_red->getText(),
                             reg_blue->getText());
    //qDebug()<<sData;
    //QByteArray data(sData.toUtf8());
    //datagram->setData(data);
    //socketDataToVideo->writeDatagram(*datagram);
}

void PCScreen::saveConditionRate(int rate)
{
    settings->beginGroup("rates");
    if(sender()->objectName() == "ball_red")
        settings->setValue("rateRed", rate);
    else
        settings->setValue("rateBlue", rate);
    settings->endGroup();
}

void PCScreen::saveConditionRules(QString np)
{
    settings->beginGroup("rates");
    if(sender()->objectName() == "np_red")
        settings->setValue("npRed", np);
    else
        settings->setValue("npBlue", np);
    settings->endGroup();
}

void PCScreen::saveConditionNames(QString str)
{
    QString objName = sender()->objectName();
    settings->beginGroup("names");
    if(objName == "fam_red")
        settings->setValue("nameRed", str);
    if(objName == "fam_blue")
        settings->setValue("nameBlue", str);
    if(objName == "fam_next_red")
        settings->setValue("nameNextRed", str);
    if(objName == "fam_next_blue")
        settings->setValue("nameNextBlue", str);
    if(objName == "reg_red")
        settings->setValue("regionRed", str);
    if(objName == "reg_blue")
        settings->setValue("regionBlue", str);
    settings->endGroup();
}

void PCScreen::saveConditionPlus(QString str)
{
    QString objName = sender()->objectName();
    settings->beginGroup("rates");
    if(objName == "plus_red"){
        if(str == "+")
            settings->setValue("plus", "red");
        else
            settings->setValue("plus", "");
    }
    else{
        if(str == "+")
            settings->setValue("plus", "blue");
        else
            settings->setValue("plus", "");
    }
    settings->endGroup();
}

void PCScreen::turnTranslationToVk(bool check)
{
    if(check){
        qDebug()<<ui.leKeyVK->text();
        QStringList arguments;
        //arguments<<"C:/Users/Colorfull/Desktop/CANON/1.MOV"<<"-vf"<<"scale=400:-1";
        arguments<<"-i"<<ui.leCam1->text()<<"-c:v"<<"h264"<<"-c:a"<<"aac"<<"-f"<<"tee"<<"-map"<<"0:v"<<"-map"<<"0:a";
        arguments<<"[f=mpegts]udp://127.0.0.1:3333|[f=flv]rtmp://ovsu.mycdn.me/input/" + ui.leKeyVK->text();

        proc = new QProcess(this);
        proc->start("ffmpeg", arguments);
    }
    else{
        proc->terminate();
    }
}

//void PCScreen::tvFullScreen(bool b){
//    if(b)
//        tvScreen->showFullScreen();
//    else
//        tvScreen->showNormal();
//}

void PCScreen::resizeEvent(QResizeEvent *){
    minimum_height = (height() - 12) / 42;

    percent_height = (height() - 12) / 100;
    QFont f;
    f.setPixelSize(cat->height() * 0.8);
    cat->setFont(f);
    age->setFont(f);

    lblEndTimer->setGeometry(0, 0, width(), height());

}

void PCScreen::setSec(int m) {
    tvScreen->grid->removeWidget(tvScreen->sec);
    if(m == 3){
        tvScreen->grid->addWidget(tvScreen->sec, 18, 25, 14, 18);
    }else if(m == 2){
        tvScreen->grid->addWidget(tvScreen->sec, 19, 25, 12, 18);
    }else{
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
    choosingNames->setNames(lf->getSportsmens());
    choosingNames->setAge(lf->lAge);
    choosingNames->setWeight(lf->lWeight);
}

void PCScreen::showListSportsmens(){
    choosingNames->showMaximized();
}

void PCScreen::changeFontWeight(QString s){
    QFile font_size("font_size.txt");

    settings->beginGroup("font");
    settings->setValue("font_size", s);
    settings->endGroup();

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
    baDatagram.append("hello display!");
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
        qDebug()<<address;
        flagUdp = 0;
        udpTimer->start(1000);
    }

}

void PCScreen::udpSend(){
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

        baDatagram.append(data.toUtf8());
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
    if(QString(baDatagram) == "ok tablo!"){
        flagUdp = 1;
        udpTimer->start(500);
        *remoteAddress = addr;
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

void PCScreen::setTvScreenGeometry(){
    QScreen* screen = QGuiApplication::primaryScreen();
    if(QGuiApplication::screens().count() == 1)
        tvScreen->setGeometry(0, 0, screen->availableGeometry().width() / 2,
                                                     screen->availableGeometry().height() / 2);
    else{
        QRect rec = QRect(QGuiApplication::screens().at(0)->availableGeometry());
        tvScreen->setGeometry(width(), 0, 100, height());
        tvScreen->setGeometry(screen->availableGeometry().right(),
                              0,
                              rec.width(),
                              rec.height());
    }
}

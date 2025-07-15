#include <QPainter>
#include <QGridLayout>
#include <QPushButton>
#include <QEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QDebug>
#include <QScreen>

#include <QMessageBox>
#include <QPalette>

#include <QFileDialog>
#include <QTime>
#include "pcscreen.h"
#include "QAction"


#include <fileapi.h>
#include <math.h>

#include <QHttpServer>
#include <QHttpServerResponse>

#include <QNetworkInterface>

#include <QRadioButton>
#include <minwindef.h>
//#include <winnt.h>
#include <Windows.h>


WidgetFilter::WidgetFilter(QObject* pobj) : QObject(pobj){
}

WidgetFilter::~WidgetFilter(){
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

    fileSettings = "settings.ini";

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

    // cbAddDisp = new QCheckBox("дополнительный\nдисплей", this);     //передача данных для дополнительного дисплея
    // cbAddDisp->setStyleSheet("color: white");
    // connect(cbAddDisp, SIGNAL(stateChanged(int)), this, SLOT(addDisplay(int)));

    SVGPushButton* doctor = new SVGPushButton(":/images/doctor.svg");
    //doctor->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    connect(doctor, SIGNAL(clicked()), this, SLOT(turnDoctor()));

    SVGPushButton * btnParter_red = new SVGPushButton(":/images/parter_red.svg");
    btnParter_red->setObjectName("btnParter_red");
    //btnParter_red->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    SVGPushButton * btnTime = new SVGPushButton(":/images/time.svg");
    btnTime->setObjectName("btnTime");
    //btnTime->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    SVGPushButton * btnParter_blue = new SVGPushButton(":/images/parter_blue.svg");
    btnParter_blue->setObjectName("btnParter_blue");
    //btnParter_blue->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    SVGPushButton * btnTehTime_red = new SVGPushButton(":/images/ttech_red.svg");
    //IconButton * btnTehTime_red = new IconButton(":/images/ttech_red.svg");
    btnTehTime_red->setObjectName("btnTehTime_red");
    //btnTehTime_red->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    //btnTehTime_red->setStyleSheet("color: red; font: bold " + QString::number(round(btnTehTime_red->height() / 2)) + "px;");

    SVGPushButton* btnSettings = new SVGPushButton(":/images/sportsmens.svg");
    //IconButton * btnSettings = new IconButton(":/images/sportsmens.svg");
    btnSettings->setObjectName("btnSettings");
    //btnSettings->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    connect(btnSettings, SIGNAL(clicked()), this, SLOT(showListSportsmens()));

    SVGPushButton * btnTehTime_blue = new SVGPushButton(":/images/ttech_blue.svg");
    btnTehTime_blue->setObjectName("btnTehTime_blue");
    //btnTehTime_blue->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    SVGPushButton * btnPlus_red = new SVGPushButton(":/images/plus_red.svg");
    btnPlus_red->setObjectName("btnPlus_red");
    //btnPlus_red->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    //btnPlus_red->setStyleSheet("color: red; font: bold " + QString::number(btnPlus_red->height()) + "px;");

    SVGPushButton * btnPlus_blue = new SVGPushButton(":/images/plus_blue.svg");
    btnPlus_blue->setObjectName("btnPlus_blue");
    //btnPlus_blue->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    //btnPlus_blue->setStyleSheet("color: blue; font: bold " + QString::number(btnPlus_blue->height()) + "px;");

    plus_red = new Plus(col_red, this);
    plus_red->setObjectName("plus_red");
    plus_red->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    plus_blue = new Plus(col_blue, this);
    plus_blue->setObjectName("plus_blue");
    plus_blue->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    mainTimer = new LCDTimer(this);
    mainTimer->setObjectName("mainTimer");

    sec_doctor = new LCDStopwatch(this, "2:01", QColor(255, 255, 0), QColor(255, 255, 0), true, true);
    sec_doctor->hide();

    sec_red = new LCDStopwatch(this, "0:20", QColor(255, 0, 0), QColor(255, 102, 102), true, true, true);
    sec_red->setObjectName("sec_red");
    sec_red->hide();

    sec_blue = new LCDStopwatch(this, "0:20", QColor(0, 0, 255), QColor(102, 102, 255), true, true, true);
    sec_blue->setObjectName("sec_blue");
    sec_blue->hide();

    sec_red_t = new LCDStopwatch(this, "2:01", QColor(255, 0, 0), QColor(255, 102, 102), true, true);
    sec_red_t->setObjectName("sec_red_t");
    sec_red_t->hide();

    sec_blue_t = new LCDStopwatch(this, "2:01", QColor(0, 0, 255), QColor(102, 102, 255), true, true);
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

    formView = new QWidget;
    ui.setupUi(formView);
    WidgetFilter* wf = new WidgetFilter(formView);
    formView->installEventFilter(wf);
    connect(wf, SIGNAL(sigClose()), this, SLOT(closeView()));

    formVideoSettings = new QWidget;
    uiVideoSettings.setupUi(formVideoSettings);
    WidgetFilter* fVideo = new WidgetFilter(formVideoSettings);
    formVideoSettings->installEventFilter(fVideo);
    connect(fVideo, SIGNAL(sigClose()), this, SLOT(closeVideoSettings()));

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
    connect(mainwin->winVideoSettings, SIGNAL(triggered()), this, SLOT(showVideoSettings()));

    connect(ui.sbSec, SIGNAL(valueChanged(int)), this, SLOT(setSec(int)));

    //connect(uiVideoSettings.cbAutoCam1, SIGNAL(toggled(bool)), this, SLOT(autoCamera(bool)));
    //connect(uiVideoSettings.cbAutoCam2, SIGNAL(toggled(bool)), this, SLOT(autoCamera(bool)));

    QLabel* lbl2 = new QLabel("Установка времени боя - 'F1', сброс - 'Backspace', выход - 'Esc'");
    lbl2->setAlignment(Qt::AlignCenter);
    lbl2->setStyleSheet("color: white; font-size: 12pt");

    QLabel* lbl3 = new QLabel("Добавить спортсмена - 'F2'");

    lbl3->setStyleSheet("color: white; font-size: 12pt");

    connect(mainwin->time, SIGNAL(triggered()), this, SLOT(setTimeFight()));

    connect(mainwin->closeProg, SIGNAL(triggered()), this, SLOT(closeTablo()));

    connect(mainwin->actSbros, SIGNAL(triggered()), this, SLOT(resetTablo()));

    QString styleTurnCamera(("QCheckBox::indicator:unchecked{image: url(:/images/CheckBoxVideoUnchecked.png)}"
                             "QCheckBox::indicator:checked{image: url(:/images/CheckBoxVideoChecked.png)}"
                             "QCheckBox::indicator {width: 45px; height: 45px;}"
                             "QCheckBox{color: white; font-size: 20px}"));


    videoControl = new VideoReplayControl(this);


    grid = new QGridLayout(this);
    grid->setObjectName("grid");

    grid->setSpacing(6);

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

    grid->addWidget(age,                    10, 26, 2,  7);
    grid->addWidget(cat,                    10, 35, 2,  7);

    grid->addWidget(videoControl,          13, 24, 12,  20);


    grid->addWidget(btnParter_red,          26, 24, 2,  6);
    grid->addWidget(btnTime,                26, 31, 2,  6);
    grid->addWidget(btnParter_blue,         26, 38, 2,  6);

    grid->addWidget(mainTimer,              28, 24, 14, 20);
    grid->addWidget(sec_doctor,             28, 24, 14, 20);

    grid->addWidget(sec_red,                11,  0,  12, 24);
    grid->addWidget(sec_blue,               11,  44, 12, 24);

    grid->addWidget(sec_red_t,              11,  0,  12, 24);
    grid->addWidget(sec_blue_t,             11,  44, 12, 24);

    grid->addWidget(fam_next_red,           42,  0,  4,  34);
    grid->addWidget(fam_next_blue,          42,  34, 4,  34);

    grid->addWidget(doctor,                 29,  45, 2,   6);

    //grid->addWidget(cbAddDisp,              28,  4, 4,   20);

    //btnTime->setStyleSheet("color: green; font: bold " + QString::number(round(btnTime->height() / 2)) + "px;");

    lblEndTimer = new EndTime(this);
    lblEndTimer->setObjectName("lblEndTimer");

    if(QGuiApplication::screens().count() == 1){
        QMessageBox::information(this, "ВНИМАНИЕ!",
                                 "Подключите к компьютеру дисплей в режиме \"Расширенный рабочий стол!\"",
                                 QMessageBox::Ok);
    }

    tvScreen = new TVScreen;
    videoControl->setPlayerTv(tvScreen->getPlayer());

    connect(videoControl, &VideoReplayControl::sigShowPlayer, tvScreen, &TVScreen::showPlayer);
    connect(videoControl, &VideoReplayControl::sigHidePlayer, tvScreen, &TVScreen::hidePlayer);
    connect(mainwin->winVideoSettings, &QAction::triggered, videoControl, &VideoReplayControl::showVideoSettings);
    //connect(mainwin->winVideoSettings, SIGNAL(triggered()), videoControl, SLOT(showVideoSettings()));
    //connect(videoControl, &VideoReplayControl::sigImage, tvScreen->player, &PlayerViewerTV::draw_image);
    connect(videoControl, &VideoReplayControl::sigShowReplayOnTv, tvScreen, &TVScreen::setPlayerEnabled);

    setTvScreenGeometry();

    //tvScreen->show();

    connect(rateRed,	SIGNAL(sigRate(int)),		 tvScreen->ball_red,	  SLOT(setRate(int)));
    connect(rateRed,    &Rate::sigRate, this, [this](int rate){
        QFile file("rate_red.txt");
        file.open(QIODevice::WriteOnly);
        file.resize(0);
        if(flagPlus == 1)
            file.write(("+" + QString::number(rate)).toUtf8());
        else
            file.write(QString::number(rate).toUtf8());
        file.close();
    });

    connect(rateBlue,	SIGNAL(sigRate(int)),		 tvScreen->ball_blue,     SLOT(setRate(int)));
    connect(rateBlue,   &Rate::sigRate, this, [this](int rate){
        QFile file("rate_blue.txt");
        file.open(QIODevice::WriteOnly);
        file.resize(0);
        if(flagPlus == 2)
            file.write(("+" + QString::number(rate)).toUtf8());
        else
            file.write(QString::number(rate).toUtf8());
        file.close();
    });

    connect(np_red,		SIGNAL(ball(QString)),	     tvScreen->np_red,		  SLOT(setValue(QString)));
    connect(np_blue,	SIGNAL(ball(QString)),	     tvScreen->np_blue,	      SLOT(setValue(QString)));

    connect(plus_red,   SIGNAL(textChange(QString)), tvScreen->plus_red,      SLOT(setData(QString)));
    connect(plus_red,   &Plus::textChange, [this](QString text){
        QFile file("rate_red.txt");
        file.open(QIODevice::WriteOnly);
        file.resize(0);
        if(text == "+"){
            flagPlus = 1;
            file.write(("+" + rateRed->getRate()).toUtf8());
        }
        else{
            flagPlus = 0;
            file.write(rateRed->getRate().toUtf8());
        }
        file.close();
    });

    connect(plus_blue,  SIGNAL(textChange(QString)), tvScreen->plus_blue,     SLOT(setData(QString)));
    connect(plus_blue,  &Plus::textChange, [this](QString text){
        QFile file("rate_blue.txt");
        file.open(QIODevice::WriteOnly);
        file.resize(0);
        if(text == "+"){
            flagPlus = 2;
            file.write(("+" + rateBlue->getRate()).toUtf8());
        }
        else{
            flagPlus = 0;
            file.write(rateBlue->getRate().toUtf8());
        }
        file.close();
    });

    connect(fam_red,       SIGNAL(sigText(QString)), tvScreen->fam_red,       SLOT(Text(QString)));
    connect(fam_blue,      SIGNAL(sigText(QString)), tvScreen->fam_blue,      SLOT(Text(QString)));

    connect(reg_red,       SIGNAL(sigText(QString)), tvScreen->reg_red,       SLOT(Text(QString)));
    connect(reg_red,    &Fam::sigText, this, [this](QString text){
        QFile file("name_red.txt");
        file.open(QIODevice::WriteOnly);
        file.resize(0);
        if(!mainwin->getStatusRegistration()){
            file.write(fam_red->getText().toUtf8() + "\n");
            file.write(text.toUtf8());
        }
        file.close();
    });

    connect(reg_blue,      SIGNAL(sigText(QString)), tvScreen->reg_blue,      SLOT(Text(QString)));
    connect(reg_blue,    &Fam::sigText, this, [this](QString text){
        QFile file("name_blue.txt");
        file.open(QIODevice::WriteOnly);
        file.resize(0);
        if(!mainwin->getStatusRegistration()){
            file.write(fam_blue->getText().toUtf8() + "\n");
            file.write(text.toUtf8());
        }
        file.close();
    });

    connect(fam_next_red,  SIGNAL(sigText(QString)), tvScreen->fam_next_red,  SLOT(Text(QString)));
    connect(fam_next_blue, SIGNAL(sigText(QString)), tvScreen->fam_next_blue, SLOT(Text(QString)));

    connect(fam_next_blue, SIGNAL(sigText(QString)), tvScreen->fam_next_blue, SLOT(Text(QString)));

    connect(mainTimer, SIGNAL(sigTime(QString, QString)), tvScreen->sec, SLOT(showTime(QString, QString)));
    connect(mainTimer, SIGNAL(sigIntTime(int)), this, SLOT(saveTime(int)));


    connect(mainTimer, SIGNAL(sigStarted(bool)), tvScreen->logo, SLOT(off_logo()));

    connect(uiTime.dMin, SIGNAL(valueChanged(int)), this, SLOT(setTime()));
    connect(uiTime.dSec, SIGNAL(valueChanged(int)), this, SLOT(setTime()));
    connect(uiTime.dSec2, SIGNAL(valueChanged(int)), this, SLOT(setTime()));

    connect(sec_red, SIGNAL(sigVisible(bool)), tvScreen->sec_red, SLOT(setVisible(bool)));
    connect(sec_red,  SIGNAL(sigTime(QString, QString)), tvScreen->sec_red, SLOT(showTime(QString, QString)));

    connect(sec_blue, SIGNAL(sigVisible(bool)), tvScreen->sec_blue, SLOT(setVisible(bool)));
    connect(sec_blue,  SIGNAL(sigTime(QString, QString)), tvScreen->sec_blue, SLOT(showTime(QString, QString)));

    connect(sec_red_t, SIGNAL(sigVisible(bool)), tvScreen->sec_red_t, SLOT(setVisible(bool)));
    connect(sec_red_t,  SIGNAL(sigTime(QString, QString)), tvScreen->sec_red_t, SLOT(showTime(QString, QString)));

    connect(sec_blue_t, SIGNAL(sigVisible(bool)), tvScreen->sec_blue_t, SLOT(setVisible(bool)));
    connect(sec_blue_t,  SIGNAL(sigTime(QString, QString)), tvScreen->sec_blue_t, SLOT(showTime(QString, QString)));

    connect(sec_doctor,  SIGNAL(sigTime(QString, QString)), tvScreen->sec_doctor, SLOT(showTime(QString, QString)));

    uiVideoSettings.leCam1->setObjectName("leCam1");
    uiVideoSettings.leCam2->setObjectName("leCam2");
    //connect(uiVideoSettings.leCam1, SIGNAL(textEdited(QString)), this, SLOT( setCam(QString)));
    //connect(uiVideoSettings.leCam2, SIGNAL(textEdited(QString)), this, SLOT(setCam(QString)));

    // settings->beginGroup("URL");
    // cam1Url = settings->value("cam1", "").toString();
    // cam2Url = settings->value("cam2", "").toString();
    // settings->endGroup();

    connect(mainTimer, SIGNAL(sigStarted(bool)), this, SLOT(StartRecord(bool)));
    connect(mainTimer, SIGNAL(sigReset()), this, SLOT(StopRecord()));

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

    //connect(uiVideoSettings.cbShowOnTv, SIGNAL(toggled(bool)), tvScreen, SLOT(setPlayerEnabled(bool)));
    //connect(uiVideoSettings.btnRefresh, SIGNAL(clicked()), this, SLOT(refreshWebCam()));

    // QString cam;
    // QString param;
    // QString sound;
    // settings->beginGroup("webcam");
    // cam =   settings->value("cam", "").toString();
    // param = settings->value("param", "").toString();
    // sound = settings->value("sound", "").toString();
    // settings->endGroup();

    // QString urlVk;
    // QString keyVk;
    // bool streamToVk;
    // int camToVk;

    // settings->beginGroup("vk");
    // urlVk       = settings->value("url", "").toString();
    // videoControl->setUrlVk(urlVk);
    // uiVideoSettings.leUrl->setText(urlVk);

    // keyVk       = settings->value("key", "").toString();
    // videoControl->setKeyVk(keyVk);
    // uiVideoSettings.leKey->setText(keyVk);

    // streamToVk  = settings->value("isTurn", false).toBool();
    // videoControl->turnStreamToVk(streamToVk);
    // uiVideoSettings.chbVk->setChecked(streamToVk);

    // camToVk     = settings->value("cam", 1).toInt();
    // videoControl->setCamToVk(camToVk);
    // if(camToVk == 1)
    //     uiVideoSettings.rbVK1->setChecked(true);
    // else if(camToVk == 2)
    //     uiVideoSettings.rbVK2->setChecked(true);
    // else
    //     uiVideoSettings.rbVK3->setChecked(true);

    // QString w = settings->value("widthPip", "1920/4").toString();
    // //videoControl->setWidthPipVk(w);
    // uiVideoSettings.leWidthPip->setText(w);

    // QString h = settings->value("heightPip", "1080/4").toString();
    // videoControl->setHeightPipVk(h);
    // uiVideoSettings.leHeightPip->setText(h);

    // QString t = settings->value("transparentPip", "0.5").toString();
    // videoControl->setTransparentPipVk(t);
    // uiVideoSettings.sbTransparent->setValue(t.toDouble());

    // QString delay = settings->value("delayPicture", "7").toString();
    // videoControl->setDelayPicture(delay);
    // uiVideoSettings.sbPicture->setValue(delay.toInt());

    // delay = settings->value("delaySound", "5").toString();
    // videoControl->setDelaySound(delay);
    // uiVideoSettings.sbPicture->setValue(delay.toInt());

    // settings->endGroup();

    // connect(uiVideoSettings.chbVk, &QCheckBox::toggled, this, [this](bool b){
    //     videoControl->turnStreamToVk(b);
    //     settings->beginGroup("vk");
    //     settings->setValue("isTurn", b);
    //     settings->endGroup();
    // });

    // connect(uiVideoSettings.rbVK1, &QRadioButton::toggled, this, [this](bool b){
    //     if(b){
    //         videoControl->setCamToVk(1);
    //         settings->beginGroup("vk");
    //         settings->setValue("cam", 1);
    //         settings->endGroup();
    //     }
    // });

    // connect(uiVideoSettings.rbVK2, &QRadioButton::toggled, this, [this](bool b){
    //     if(b){
    //         videoControl->setCamToVk(2);
    //         settings->beginGroup("vk");
    //         settings->setValue("cam", 2);
    //         settings->endGroup();
    //     }
    // });

    // connect(uiVideoSettings.rbVK3, &QRadioButton::toggled, this, [this](bool b){
    //     if(b){
    //         videoControl->setCamToVk(3);
    //         settings->beginGroup("vk");
    //         settings->setValue("cam", 3);
    //         settings->endGroup();
    //     }
    // });

    // connect(uiVideoSettings.leUrl, &QLineEdit::editingFinished, this, [this](){
    //     videoControl->setUrlVk(uiVideoSettings.leUrl->text());
    //     settings->beginGroup("vk");
    //     settings->setValue("url", uiVideoSettings.leUrl->text());
    //     settings->endGroup();
    // });

    // connect(uiVideoSettings.leKey, &QLineEdit::editingFinished, this, [this](){
    //     videoControl->setKeyVk(uiVideoSettings.leKey->text());
    //     settings->beginGroup("vk");
    //     settings->setValue("key", uiVideoSettings.leKey->text());
    //     settings->endGroup();
    // });

    // connect(uiVideoSettings.cbWebCam, &QComboBox::currentTextChanged, [this](QString text){
    //     //disconnect(uiVideoSettings.cbParamWebCam);
    //     uiVideoSettings.cbParamWebCam->clear();
    //     if(uiVideoSettings.cbWebCam->count() > 0){
    //         videoControl->setWebCam(text);
    //         QList<QList<int>> param = Player::getListParamWebCam(text);
    //         qDebug()<<"param = "<<param;
    //         int count = 0;

    //         foreach(auto each, param){
    //             QString sParam = "fps = " + QString::number(each.at(0)) +
    //                              "resolution = " + QString::number(each.at(1)) +
    //                              "x" + QString::number(each.at(2));
    //             uiVideoSettings.cbParamWebCam->addItem(sParam);
    //             uiVideoSettings.cbParamWebCam->setItemData(count++, QVariant::fromValue(each));
    //         }
    //         setParamWebCam(uiVideoSettings.cbParamWebCam->currentIndex());
    //     }
    //     //connect(uiVideoSettings.cbParamWebCam, &QComboBox::currentTextChanged, this, &PCScreen::setParamWebCam);
    // });
    //connect(uiVideoSettings.cbParamWebCam, QOverload<int>::of(&QComboBox::activated), this, &PCScreen::setParamWebCam);

    // connect(uiVideoSettings.cbSound, &QComboBox::currentTextChanged, this, [this](QString text){
    //     videoControl-> setSound(text);
    //     settings->beginGroup("webcam");
    //     settings->setValue("sound", text);
    //     settings->endGroup();
    // });

    // connect(uiVideoSettings.leWidthPip, &QLineEdit::editingFinished, this, [this](){
    //     videoControl->setWidthPipVk(uiVideoSettings.leWidthPip->text());
    //     settings->beginGroup("vk");
    //     settings->setValue("widthPip", uiVideoSettings.leWidthPip->text());
    //     settings->endGroup();
    // });

    // connect(uiVideoSettings.leHeightPip, &QLineEdit::editingFinished, this, [this](){
    //     videoControl->setHeightPipVk(uiVideoSettings.leHeightPip->text());
    //     settings->beginGroup("vk");
    //     settings->setValue("heightPip", uiVideoSettings.leHeightPip->text());
    //     settings->endGroup();
    // });

    // connect(uiVideoSettings.sbTransparent, &QDoubleSpinBox::textChanged, this, [this](){
    //     videoControl->setTransparentPipVk(uiVideoSettings.sbTransparent->text());
    //     settings->beginGroup("vk");
    //     settings->setValue("transparentPip", uiVideoSettings.sbTransparent->text().replace(",", "."));
    //     settings->endGroup();
    // });

    // connect(uiVideoSettings.sbPicture, &QSpinBox::textChanged, this, [this](){
    //     videoControl->setDelayPicture(uiVideoSettings.sbPicture->text());
    //     settings->beginGroup("vk");
    //     settings->setValue("delayPicture", uiVideoSettings.sbPicture->text());
    //     settings->endGroup();
    // });

    // connect(uiVideoSettings.sbSound, &QSpinBox::textChanged, this, [this](){
    //     videoControl->setDelayPicture(uiVideoSettings.sbSound->text());
    //     settings->beginGroup("vk");
    //     settings->setValue("delaySound", uiVideoSettings.sbSound->text());
    //     settings->endGroup();
    // });

    // refreshWebCam();
    // int index = uiVideoSettings.cbWebCam->findText(cam);
    // if(index != -1)
    //     uiVideoSettings.cbWebCam->setCurrentIndex(index);
    // index = uiVideoSettings.cbParamWebCam->findText(param);
    // if(index != -1)
    //     uiVideoSettings.cbParamWebCam->setCurrentIndex(index);
    // index = uiVideoSettings.cbSound->findText(sound);
    // if(index != -1)
    //     uiVideoSettings.cbSound->setCurrentIndex(index);

    setSize();

    connect(this, SIGNAL(sigLogo(bool)), tvScreen, SIGNAL(sigLogo(bool)));

    connect(mainTimer, SIGNAL(sigClicked()), btnTime, SLOT(click()));

    f_Lib = new QLibrary;

    QHttpServer httpServer;
    httpServer.route("/", []() {
        QHttpServerResponse resp = QHttpServerResponse::fromFile(QString("index.html"));
        QHttpHeaders headers;
        headers.append(QHttpHeaders::WellKnownHeader::Refresh, "1");
        resp.setHeaders(headers);
        qDebug()<<"resp";
        return resp;
    });


    auto tcpserver = std::make_unique<QTcpServer>();
    tcpserver->listen(QHostAddress::Any, 6001);
    if (!httpServer.bind(tcpserver.get())) {
        qWarning() << QCoreApplication::translate("QHttpServerExample",
                                                  "Server failed to listen on a port.");
        //return -1;
    }

    qDebug()<<tcpserver->serverPort();
    tcpserver.release();

}

// void PCScreen::refreshWebCam()
// {
//     uiVideoSettings.cbSound->clear();
//     uiVideoSettings.cbWebCam->clear();
//     uiVideoSettings.cbWebCam->addItems(Player::getListWebCams());

//     uiVideoSettings.cbSound->addItems(Player::getListSoundDevices());
// }

PCScreen::~PCScreen()
{

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

void PCScreen::setCat(QString s){
    cat->setText(s);
    tvScreen->cat->setText(s);
}

void PCScreen::setAge(QString s){
    age->setText(s);
    tvScreen->age->setText(s);
}

// void PCScreen::autoCamera(bool state){
//     if(state){
//         if(sender()->objectName() == "cbAutoCam1"){
//             uiVideoSettings.cbAutoCam2->setEnabled(false);
//             camConn = new CameraConnection(this);
//             uiVideoSettings.leCam1->setText("");
//             uiVideoSettings.leCam1->setStyleSheet("background-color: red");
//         }
//         else{
//             uiVideoSettings.cbAutoCam1->setEnabled(false);
//             camConn = new CameraConnection(this, 2);
//             uiVideoSettings.leCam2->setText("");
//             uiVideoSettings.leCam2->setStyleSheet("background-color: red");
//         }
//         connect(camConn, SIGNAL(sigCamera(QString)), this, SLOT(setCamera(QString)));
//     }
//     else{
//         if(sender()->objectName() == "cbAutoCam1"){
//             uiVideoSettings.cbAutoCam2->setEnabled(true);
//             uiVideoSettings.leCam1->setStyleSheet("background-color: white");
//             settings->beginGroup("URL");
//             uiVideoSettings.leCam1->setText(settings->value("cam1", "").toString());
//             settings->endGroup();
//             //f.close();
//         }
//         else{
//             uiVideoSettings.cbAutoCam1->setEnabled(true);
//             uiVideoSettings.leCam2->setStyleSheet("background-color: white");
//             settings->beginGroup("URL");
//             uiVideoSettings.leCam2->setText(settings->value("cam2", "").toString());
//             settings->endGroup();
//             //f.close();
//         }
//         if(camConn){
//             disconnect(camConn, SIGNAL(sigCamera(QString)), nullptr, nullptr);
//             camConn->deleteLater();
//         }
//     }
// }

void PCScreen::setCamera(QString ip){
    if(uiVideoSettings.cbAutoCam1->isChecked()){
        cam1Url = "srt://" + ip + ":1111";
        settings->beginGroup("URL");
        settings->setValue("cam1", cam1Url);
        settings->endGroup();
        uiVideoSettings.cbAutoCam1->setChecked(false);
    }
    if(uiVideoSettings.cbAutoCam2->isChecked()){
        cam2Url = "srt://" + ip + ":2222";
        settings->beginGroup("URL");
        settings->setValue("cam2", cam2Url);
        settings->endGroup();
        uiVideoSettings.cbAutoCam2->setChecked(false);
    }
}


void PCScreen::closeVideoSettings()
{
    if(uiVideoSettings.cbAutoCam1->isChecked())
        uiVideoSettings.cbAutoCam1->setChecked(false);
    if(uiVideoSettings.cbAutoCam2->isChecked())
        uiVideoSettings.cbAutoCam2->setChecked(false);
}

void PCScreen::StartRecord(bool b){
    QString redFam = fam_red->getText().split(" ").count() > 1 ? fam_red->getText().split(" ").at(0) : fam_red->getText();
    QString blueFam = fam_blue->getText().split(" ").count() > 1 ? fam_blue->getText().split(" ").at(0) : fam_blue->getText();
    videoControl->startRecord(b, redFam + "-" + blueFam);
}

void PCScreen::StopRecord(){
    videoControl->stopRecord();
}



void PCScreen::PlaySelectedFile(){
    tvScreen->showPlayer();
}





// void PCScreen::setCam(QString text){
//     if(sender()->objectName() == "leCam1"){
//         cam1Url = text;
//         settings->beginGroup("URL");
//         settings->setValue("cam1", cam1Url);
//         settings->endGroup();
//         videoControl->setCam2(cam1Url);
//     }else {
//         cam2Url = text;
//         settings->beginGroup("URL");
//         settings->setValue("cam2", cam2Url);
//         settings->endGroup();
//         videoControl->setCam3(cam2Url);
//     }
// }

void PCScreen::closeEvent(QCloseEvent *){
    videoControl->deleteLater();
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
    formView->show();
}

// void PCScreen::showVideoSettings()
// {
//     settings->beginGroup("URL");
//     uiVideoSettings.leCam1->setText(settings->value("cam1", "").toString());
//     settings->endGroup();

//     settings->beginGroup("URL");
//     uiVideoSettings.leCam2->setText(settings->value("cam2", "").toString());
//     settings->endGroup();

//     formVideoSettings->show();
// }

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
        choosingNames->showMaximized();
    }else{
        emit sendKey(pe->key());
    }
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
    QString sData(sTime + ";" + fam_red->getText() + ";" + reg_red->getText() + ";" + rateRed->text()
                  + ";" + fam_blue->getText() + ";" + reg_blue->getText() + ";" + rateBlue->text());
    QByteArray data(sData.toUtf8());
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

        baDatagram.append(data.toStdString());
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

void PCScreen::setTvScreenGeometry(){
    if(QGuiApplication::screens().count() == 2){
        QList<QScreen*> lScreens = QGuiApplication::screens();
        qDebug()<<lScreens<<lScreens.at(0)->availableGeometry()<<lScreens.at(1)->availableGeometry();
        tvScreen->setGeometry(lScreens.at(1)->availableGeometry());
        tvScreen->show();
        tvScreen->showFullScreen();
    }
    else{
        tvScreen->setGeometry(0, 0, QApplication::primaryScreen()->availableGeometry().width() / 2,
                              QApplication::primaryScreen()->availableGeometry().height() / 2);
        tvScreen->show();
    }


}

// void PCScreen::setCameras()
// {
//     videoControl->setWebCam(uiVideoSettings.cbWebCam->currentText() + ";" + uiVideoSettings.cbParamWebCam->currentText());
// }

// void PCScreen::setParamWebCam(int index)
// {
//     QVariant variant = uiVideoSettings.cbParamWebCam->itemData(index);
//     QList<int> data = variant.value<QList<int>>();
//     videoControl->setParamWebCam(data);
//     settings->beginGroup("webcam");
//     settings->setValue("cam", uiVideoSettings.cbWebCam->currentText());
//     settings->setValue("param", uiVideoSettings.cbParamWebCam->currentText());
//     settings->endGroup();
// }

 #pragma once
#include <QWidget>
#include <QPushButton>
#include "lcdtimer.h"
#include <QGridLayout>
#include <QLabel>
#include "narusheniya_pravil.h"
#include "rate.h"
#include "ui_frmTime.h"
#include "ui_FormView.h"
//#include "ui_frmTv.h"
#include "fam_reg.h"
#include "plus.h"
#include "tvscreen.h"
#include "cameraviewer.h"
#include "camera.h"
#include "playerviewer.h"
#include <QPointer>
#include <QCheckBox>
#include "cameraconnection.h"
#include "lcdstopwatch.h"
#include "endtime.h"
//#include "newsportsman.h"
#include "list_family.h"
#include <mainwindow.h>
#include <QSettings>
#include <QNetworkDatagram>

#include <QSvgRenderer>
#include <QProcess>
#include "choosingnames.h"

class IconButton : public QPushButton{

public:
        IconButton(const QString& iconPath){
            path = iconPath;
        }

    void paintEvent(QPaintEvent* ev){
        QPushButton::paintEvent(ev);
        QPainter painter(this);
        QSvgRenderer renderer(path);
        renderer.render(&painter);

    }

private:
    QString path;

};

class WidgetFilter : public QObject {
    Q_OBJECT

public:
    WidgetFilter(QObject* pobj = nullptr);

    ~WidgetFilter();

    virtual bool eventFilter(QObject*, QEvent*);

signals:
    void sigClose(void);
    //void setCategory(void);

};

class PCScreen : public QWidget {
	Q_OBJECT
public:
    explicit PCScreen(MainWindow*, QWidget *parent = 0);
    MainWindow* mainwin;

    ~PCScreen();

    EndTime * lblEndTimer;         //красный фон по окончании времени


signals:
	void sendKey(int);
    //void setCategory(void);
    //void signalReplay(void);
    //void changeViewName(int);
    void sig_hide(QString, QString, QString, QString);
    void sigLogo(bool);

public slots:
    //void setFrameWidth(int);
    //void setSpace(int);
	void setSec(int);
    void setTime(void);
    void newListSportsmens(void);
    void sbrosLogo(void);
    void Variant(int);
    void setTvScreenGeometry(void);
    void initListNames(void);   //инициализация списков спортсменов
    void slotExit(void);

private slots:
    void setCam(void);
    void showView(void);

    void finishedCamera(void);
    void turnCamera(bool);
    //void showSettings(void);
    //void reset(void);

    void StopRecord(void);
    void StartRecord(bool);
    void PlayFile(void);
    void PlaySlowMotion(void);
    void closePlayer(void);
    void PlaySelectedFile(void);
    void changeSize(void);
    void drawTvScreenshot(void);

    void autoCamera(bool);
    void closeView(void);
    void setCamera(QString);

    void setCat(QString);
    void setAge(QString);

    void CpuUsage(void);

    void HIDE(QString, QString, QString, QString);

    void changeFontWeight(QString);

    void turnDoctor(void);

    void addDisplay(int);

    void setAddress(bool);

    void udpSend(void);

    void slotProcessDatagrams(void);

    void showListSportsmens(void);

    void setTimeFight(void);
    void closeTablo(void);
    void resetTablo(void);

    void closeWinName(QString, QString, QString, QString, QString, QString, QString, QString);
    //void tvXchange(int);
    //void tvYchange(int);
    //void tvWchange(int);
    //void tvHchange(int);
    //void tvReset(void);
    //void tvFullScreen(bool);
    void delListNames(void);
    void saveTime(int);
    void saveConditionRate(int);
    void saveConditionRules(QString);
    void saveConditionNames(QString);
    void saveConditionPlus(QString);

private:
    //QLabel* lblCpuUsage;
    QCheckBox* cbAddDisp;
    QSettings* settings;
    QString fileSettings;
	QWidget * formView;
    QWidget * frmTime;
    LCDStopwatch * sec_doctor;
    QWidget* frmTvSettings;
    //Ui::frmTV uiTV;
    QProcess* myProcess;

	virtual void paintEvent(QPaintEvent *);
    virtual void closeEvent(QCloseEvent*);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void resizeEvent(QResizeEvent *);

    int screenTop=0;
    int screenLeft=0;
    int screenWidth=0;
    int screenHeight=0;

    void setSize(void);

    //NewSportsman* newSportsman;

    ListFamily * lf = nullptr;

    ChoosingNames* choosingNames = nullptr;

    //void process_line(int, QString);

	QString col_red;
	QString col_blue;
    //QEvent * ev_L;
    //QEvent * ev_R;
	int minimum_height_family;
	int minimum_height_region;
	QGridLayout * grid;
	NP * np_red;
	NP * np_blue;

    Rate * rateRed;
    Rate * rateBlue;
    //Rate * actRed;
    //Rate * actBlue;
	Fam * fam_red;
	Fam * fam_blue;
	Fam * reg_red;
	Fam * reg_blue;
    Fam * fam_next_red;
    Fam * fam_next_blue;
	Plus * plus_red;
	Plus * plus_blue;

    LCDTimer * mainTimer;
    LCDStopwatch * sec_red;
    LCDStopwatch * sec_blue;
    LCDStopwatch * sec_red_t;
    LCDStopwatch * sec_blue_t;

	Ui::frmView ui;
    Ui::Form uiTime;
	QDesktopWidget* desk;
	int HEIGHT_REGION;
	int HEIGHT_FAMILY;
    int HEIGHT_NEXT_FAMILY;
	int minimum_height;
	int percent_height;
    TVScreen * tvScreen;
    QPushButton * cat;  //категория
    QPushButton * age;  //возраст

    CameraViewer* viewCam1;
    CameraViewer* viewCam2;
    Camera* camera1;
    Camera* camera2;
    QThread* threadCam1;
    QThread* threadCam2;
    QDialog * dialogSetUrl;
    QCheckBox* cbCam1;
    QCheckBox* cbCam2;
    QPointer<PlayerViewer> slowMotionPlayer;

    QPushButton* btnStopRecord;
    QPushButton* btnPlayLastWithSound1;
    QPushButton* btnPlayLastSlowMotion1;
    QPushButton* btnPlayLastWithSound2;
    QPushButton* btnPlayLastSlowMotion2;
    QPushButton* btnPlaySlowMotion;
    QLabel* lbl;

    QString cam1Url = "";
    QString cam2Url = "";

    //QString currentFileCam1 = "";
    //QString currentFileCam2 = "";


    //Ui::dlgSetUrl ui_url;


    QThread* cam1Thread;
    QThread* cam2Thread;

    QPointer<CameraConnection> camConn;

    QString address;
    QHostAddress* remoteAddress;
    QUdpSocket* s_udp;
    QTimer* udpTimer;
    int flagUdp;    //0 - запрос адреса удаленного компьютера
                    //1 - отправка данных

    QUdpSocket* socketDataToVideo;
    QNetworkDatagram* datagram;

};

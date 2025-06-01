 #pragma once
#include <QWidget>
#include <QPushButton>
#include "lcdtimer.h"
#include <QGridLayout>
#include <QLabel>
#include "narusheniya_pravil.h"
#include "qfuture.h"
#include "qlibrary.h"
#include "rate.h"
#include "ui_FormSettingsVideoReplay.h"
#include "ui_frmTime.h"
#include "ui_FormView.h"
#include "fam_reg.h"
#include "plus.h"
#include "tvscreen.h"
#include <QPointer>
#include <QCheckBox>
#include "cameraconnection.h"
#include "lcdstopwatch.h"
#include "endtime.h"
#include "list_family.h"
#include <mainwindow.h>
#include <QSettings>
#include <QNetworkDatagram>

#include <QSvgRenderer>
#include <QProcess>
#include "choosingnames.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QLibrary>
#include "videoreplaycontrol.h"
#include <QSvgWidget>

class SVGPushButton : public QPushButton
{
public:
    SVGPushButton(QString svgPath) : QPushButton()
{


    setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
    QSvgWidget *w = new QSvgWidget(svgPath,this);

    QHBoxLayout* box = new QHBoxLayout(this);
    box->setContentsMargins(0,0,0,0);
    setLayout( new QHBoxLayout(this) );
    layout()->addWidget( w );
}

};



class IconButton : public QPushButton{

public:
        IconButton(const QString& iconPath, const QString& iconPath2 = "", int as = 1){
            path = iconPath;
            path2 = iconPath2;
            renderer.load(path);
            if(path2 != ""){
                flag = true;
                aspectRatio = as;
            }
        }

    void paintEvent(QPaintEvent* ev){
        QPushButton::paintEvent(ev);
        QPainter painter(this);

        if(flag){
            QRect rect(width() / 2 - height() * aspectRatio / 2, 0, height() * aspectRatio, height());
            if(isEnabled())
                renderer.render(&painter, rect);
            else{
                QSvgRenderer renderer2(path2);
                renderer2.render(&painter, rect);
            }
        }
        else
            renderer.render(&painter);
    }

private:
    QString path;
    QString path2;
    bool flag;
    int aspectRatio;
    QSvgRenderer renderer;

};

class WidgetFilter : public QObject {
    Q_OBJECT

public:
    WidgetFilter(QObject* pobj = nullptr);

    ~WidgetFilter();

    virtual bool eventFilter(QObject*, QEvent*);

signals:
    void sigClose(void);

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
    void sig_hide(QString, QString, QString, QString);
    void sigLogo(bool);

public slots:
    void setSec(int);
    void setTime(void);
    void newListSportsmens(void);
    void sbrosLogo(void);

    void initListNames(void);   //инициализация списков спортсменов


private slots:
    void setCam(QString);
    void showView(void);
    void showVideoSettings(void);

    void StopRecord(void);
    void StartRecord(bool);

    void PlaySelectedFile(void);
    void changeSize(void);
    void drawTvScreenshot(void);

    void autoCamera(bool);

    void closeVideoSettings(void);
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
    void delListNames(void);
    void saveTime(int);
    void saveConditionRate(int);
    void saveConditionRules(QString);
    void saveConditionNames(QString);
    void saveConditionPlus(QString);

    void refreshWebCam(void);

    void setTvScreenGeometry(void);

    void setCameras(void);
    void setParamWebCam(int index);

private:
    QSettings* settings;
    QString fileSettings;
    QWidget * formView;
    QWidget * formVideoSettings;
    QWidget * frmTime;
    LCDStopwatch * sec_doctor;
    QWidget* frmTvSettings;

    virtual void paintEvent(QPaintEvent *);
    virtual void closeEvent(QCloseEvent*);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void resizeEvent(QResizeEvent *);

    int screenTop=0;
    int screenLeft=0;
    int screenWidth=0;
    int screenHeight=0;

    void setSize(void);

    ListFamily * lf = nullptr;

    ChoosingNames* choosingNames = nullptr;

    QString col_red;
    QString col_blue;

    int minimum_height_family;
    int minimum_height_region;
    QGridLayout * grid;
    NP * np_red;
    NP * np_blue;

    Rate * rateRed;
    Rate * rateBlue;

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
    Ui::frmSettingsVideoReplay uiVideoSettings;
    Ui::Form uiTime;
    QScreen* desk;
    int HEIGHT_REGION;
    int HEIGHT_FAMILY;
    int HEIGHT_NEXT_FAMILY;
    int minimum_height;
    int percent_height;
    TVScreen * tvScreen;
    QPushButton * cat;  //категория
    QPushButton * age;  //возраст

    QDialog * dialogSetUrl;

    QLabel* lbl;

    QString cam1Url = "";
    QString cam2Url = "";

    QThread* cam1Thread;
    QThread* cam2Thread;

    QPointer<CameraConnection> camConn;

    QString address;
    QHostAddress* remoteAddress;
    QUdpSocket* s_udp;
    QTimer* udpTimer;
    int flagUdp;    //0 - запрос адреса удаленного компьютера
                    //1 - отправка данных

    QTcpServer* server;
    QTcpSocket* socket;

    QLibrary* f_Lib;

    QFuture<QString> future;


    VideoReplayControl* videoControl;

signals:
    void operate(QString, int, QString);

};

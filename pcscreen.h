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
    explicit PCScreen(QWidget *parent = 0);

    //~PCScreen();



signals:
	void sendKey(int);
    //void setCategory(void);
    //void signalReplay(void);
    void changeViewName(int);

public slots:
    //void setFrameWidth(int);
    //void setSpace(int);
	void setSec(int);
    void setTime(void);

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

private:
    QLabel* lblCpuUsage;
	QWidget * formView;
    QWidget * frmTime;
    LCDStopwatch * sec_doctor;

	virtual void paintEvent(QPaintEvent *);
    virtual void closeEvent(QCloseEvent*);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void resizeEvent(QResizeEvent *);



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
    Rate * actRed;
    Rate * actBlue;
	Fam * fam_red;
	Fam * fam_blue;
	Fam * reg_red;
	Fam * reg_blue;
    Fam * fam_next_red;
    Fam * fam_next_blue;
	Plus * plus_red;
	Plus * plus_blue;
    LCDTimer * mainTimer;
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

    QString cam1Url = "";
    QString cam2Url = "";

    //QString currentFileCam1 = "";
    //QString currentFileCam2 = "";


    //Ui::dlgSetUrl ui_url;


    QThread* cam1Thread;
    QThread* cam2Thread;

    QPointer<CameraConnection> camConn;

};

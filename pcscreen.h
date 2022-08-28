#pragma once
#include <QWidget>
#include <QPushButton>
#include "lcdtimer.h"
#include <QGridLayout>
#include <QLabel>
#include "narusheniya_pravil.h"
#include "rate.h"
#include "narusheniya_vyhod.h"
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

class PCScreen : public QWidget {
	Q_OBJECT
public:
    explicit PCScreen(QWidget *parent = 0);

    ~PCScreen();



signals:
	void sendKey(int);
    //void setCategory(void);
    //void signalReplay(void);

public slots:
	void sendEV_L(QObject *);
	void sendEV_R(QObject *);
	void setFlagRed(QString);
	void setFlagBlue(QString);
	void setView(void);
	void setFrameWidth(int);
	void setSpace(int);
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

private:
	QWidget * formView;
    QWidget * frmTime;
	int View;									//вид табло: 0-красно-синий фон, 
												//			 1-чёрный фон с рамками,
												//			 2-чёрный фон без рамок
	virtual void paintEvent(QPaintEvent *);
    virtual void closeEvent(QCloseEvent*);
    virtual void keyPressEvent(QKeyEvent *);
	virtual void showEvent(QShowEvent *);
    virtual void resizeEvent(QResizeEvent *);

    void process_line(int, QString);

	QLabel * flag_blue;
	QLabel * flag_red;
	QString col_red;
	QString col_blue;
	QEvent * ev_L;
	QEvent * ev_R;
	int minimum_height_family;
	int minimum_height_region;
	QGridLayout * grid;
	NP * np_red;
	NP * np_blue;
	NV * nv_red;
	NV * nv_blue;
    Rate * rateRed;
    Rate * rateBlue;
    Rate * actRed;
    Rate * actBlue;
	Fam * fam_red;
	Fam * fam_blue;
	Fam * reg_red;
	Fam * reg_blue;
	Plus * plus_red;
	Plus * plus_blue;
    LCDTimer * mainTimer;
	Ui::frmView ui;
    Ui::Form uiTime;
	QDesktopWidget* desk;
	int HEIGHT_REGION;
	int HEIGHT_FAMILY;
	int minimum_height;
	int percent_height;
    TVScreen * tvScreen;
    QLabel * cat;

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

    QString currentFileCam1 = "";
    QString currentFileCam2 = "";


    //Ui::dlgSetUrl ui_url;


    QThread* cam1Thread;
    QThread* cam2Thread;

};

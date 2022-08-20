#pragma once
#include <QWidget>
#include <QPushButton>
#include "Secundomer.h"
#include <QGridLayout>
#include <QLabel>
#include "narusheniya_pravil.h"
#include "Ball.h"
#include "narusheniya_vyhod.h"
#include "ui_frmTime.h"
#include "ui_FormView.h"
#include "fam_reg.h"
#include "plus.h"
#include "Second_display.h"
#include "cameraviewer.h"
#include "ffmpegthread.h"

class FirstDisplay : public QWidget {
	Q_OBJECT
public:
	explicit FirstDisplay(QWidget *parent = 0);

	~FirstDisplay();



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
	//void changeSize(int i);
    void errorCamera(void);

    void showReplay(void);
    void start_replay(void);

    void setTime(void);



private slots:
    void turnCamera(int);
    void setCam(void);
    void showView(void);

    void test(void);
    void removeReplay(void);
    void enableReplayOnTV(int);
    void setTimeReplay(bool);
    void StopRecord(void);
    void PlayFile(void);

private:
	QWidget * formView;
    QWidget * frmTime;
	int View;									//вид табло: 0-красно-синий фон, 
												//			 1-чёрный фон с рамками,
												//			 2-чёрный фон без рамок
	virtual void paintEvent(QPaintEvent *);
	virtual void keyPressEvent(QKeyEvent *);
	virtual void showEvent(QShowEvent *);
    virtual void resizeEvent(QResizeEvent *);
	void changeSize(int);
    void process_line(int, QString);


    QPushButton* btnStopRecord;
    QPushButton* btnStartPlay;

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
	Ball * ball_red;
	Ball * ball_blue;
	Ball * akt_red;
	Ball * akt_blue;
	Fam * fam_red;
	Fam * fam_blue;
	Fam * reg_red;
	Fam * reg_blue;
	Plus * plus_red;
	Plus * plus_blue;
	Secundomer * sec;
	Ui::frmView ui;
    Ui::Form uiTime;
	QDesktopWidget* desk;
	int HEIGHT_REGION;
	int HEIGHT_FAMILY;
	int minimum_height;
	int percent_height;
	SecondDisplay * SD;
    CameraViewer* cv_left;
    CameraViewer* cv_right;

    FFmpegThread* camLeftThread;
    FFmpegThread* camRightThread;

    QThread* cam1Thread;
    QThread* cam2Thread;

    Category * cat;

    ReplayViewer* rV;
};

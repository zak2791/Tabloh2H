#pragma once
#include <QLCDNumber>

class Secundomer : public QLCDNumber {
	Q_OBJECT
	Q_PROPERTY(bool Visible READ getVisible)
public:
	explicit Secundomer(QWidget *parent = 0,
						QEvent * _evl = nullptr,
						QEvent * _evr = nullptr,
						QString _col_start = "yellow", 
						QString _col_stop = "gold",
						double __tm = 0.10, 
						int _drc = -1, 
						bool _bg = true, 
						bool _beep = true);
	~Secundomer();
	

	//void showTime();

public slots:
	void startstop();
	void clearTimer();
	bool getVisible();
	void doubleTime(QString, QPalette);
    bool isActive(void);
    void showLastTime(QString);
    QString getLastTime(void);

private slots:
	void showTime();
	
signals:
	void sec_sig(QString, QPalette);
    void sec_visible(bool);
    void startRec(void);
    void stopRec(void);

public:
	//void clearTimer();
    void setTime(int, int, int);

private:
	virtual void customEvent(QEvent * pe);
    QTimer* timer;
	//int count;
	QTime * time;
	QEvent * evl, * evr;
	//QString col_start, col_stop;
	double _tm;
	bool bg, beep;
	int fl_val, drc;
	bool fl_state;
	QPalette pal_start, pal_stop;
	QString text;
	bool Visible;
};

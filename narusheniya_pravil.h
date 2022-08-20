#pragma once

#include <QLabel>
#include <QThread>

class Blinks : public QObject {
	Q_OBJECT
public:
	Blinks();
	virtual ~Blinks();
public slots:
	void process();

signals:
	void blink_turn(bool);

private:
	bool current_state;
};

class NP : public QLabel {
	Q_OBJECT
public:
	explicit NP(QLabel *parent = 0);

	~NP();
	
	
signals:
	//void change_np(QString);
	void ball(QString);

public slots:
	void prg(bool);
	void setValue(QString);
	void sbros();

private:
	void set_red();
	void set_black();
	void plus();
	void minus();
	
	virtual void mousePressEvent(QMouseEvent *);
	virtual void resizeEvent(QResizeEvent *);
	QString bl, style, style_red, style_yellow, style_green;
	
};
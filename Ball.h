#pragma once
#include <QLabel>

class Ball : public QLabel {
	Q_OBJECT
public:
	explicit Ball(QWidget *parent = 0,
				  int _max_num = 9,
				  bool _diff_font = false);

	~Ball();

	int id = 0;

	void plus();
	void minus();

signals:
	void ball(int);

public slots:
	void sbros();
	void setBall(int);
	void setViewStyle(int, int);

private:
	virtual void mousePressEvent(QMouseEvent *);
	virtual void resizeEvent(QResizeEvent *);
	int bl = 0;
	
	int max_num;
	//int width_frame;
	bool diff_font;
	QFont fnt1, fnt2;

	//QString style_white;
	//QString style_red;
	//QString style_blue;
	//QString style_red_no_frame;
	//QString style_blue_no_frame;
};
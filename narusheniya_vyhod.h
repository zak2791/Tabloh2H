#pragma once

#include <QLabel>

class NV : public QLabel {
	Q_OBJECT
public:
	explicit NV(QString col = "white", QLabel *parent = 0);

	~NV();

	int id;
public slots:
	void sbros();
	void setValue(QString);

signals:
	//void change_nv(QString);
	void ball(QString);

private:

	void plus();
	void minus();
	
	virtual void mousePressEvent(QMouseEvent *);
	virtual void resizeEvent(QResizeEvent *);
	QString bl, style, style_yellow, style_green;
};

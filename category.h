#pragma once

#include <QLabel>

class Category : public QLabel {
    Q_OBJECT
	//Q_PROPERTY(QString Text READ getText)
public:
	explicit Category(QString col = "yellow", QWidget *parent = 0);

	~Category();

public slots:
	//QString getText();
    void setData(QString);



private:
	virtual void resizeEvent(QResizeEvent *);
	//QString Text;

signals:
    void sigText(QString);

};



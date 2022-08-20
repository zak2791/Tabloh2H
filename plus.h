#pragma once

#include <QLabel>

class Plus : public QLabel {
	Q_OBJECT
	Q_PROPERTY(QString Text READ getText)
public:
	explicit Plus(QString col = "yellow", QWidget *parent = 0);

	~Plus();

public slots:
	QString getText();
	void setColor(QString);
    void setData(QString);

private:
	virtual void resizeEvent(QResizeEvent *);
	QString Text;

signals:
    void textChange(QString);
};


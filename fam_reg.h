#pragma once
#include <QLabel>

class Fam : public QLabel {
	Q_OBJECT
    Q_PROPERTY(QString text WRITE Text)

public:
	explicit Fam(QString _cl = "yellow",
				 QString _tx = "text",
				 int _wt = 10,
				 QString fn = "",
                 QWidget *parent = 0);
		

	~Fam();

	//int id = 0;
	int align = 0;

public slots:
	void Text(QString);
	void setViewStyle(int);
    QString getText(void);
	//private slots:
		//

	//public:
	

private:
	virtual void paintEvent(QPaintEvent *);


protected:
    int wt;
    //QString fn;
    QString tx;
    QFont fm;


signals:
    void sigText(QString);
};

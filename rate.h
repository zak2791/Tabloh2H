#ifndef RATE_H
#define RATE_H

#include <QLabel>

class Rate : public QLabel {
    Q_OBJECT
public:
    Rate(QWidget *parent = 0);

    //~Rate();


    void plus();
    void minus();

signals:
    void sigRate(int);

public slots:
    void sbros();
    void setRate(int);

private:
    virtual void mousePressEvent(QMouseEvent *);
    virtual void resizeEvent(QResizeEvent *);
    int rate = 0;
    QFont largeFont, smallFont;
};

#endif

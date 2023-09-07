#ifndef LOGOFRB_H
#define LOGOFRB_H

#include <QWidget>

class logofrb : public QWidget
{
    Q_OBJECT
public:
    explicit logofrb(QWidget *parent = nullptr);

private:
    virtual void paintEvent(QPaintEvent*);

signals:

};

#endif // LOGOFRB_H

#ifndef LOGOFRB_H
#define LOGOFRB_H

#include <QWidget>

class logofrb : public QWidget
{
    Q_OBJECT
public:
    explicit logofrb(QWidget *parent = nullptr);

public slots:
    void on_logo(void);
    void off_logo(void);

private:
    virtual void paintEvent(QPaintEvent*);

};

#endif // LOGOFRB_H

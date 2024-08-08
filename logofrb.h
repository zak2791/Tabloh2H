#ifndef LOGOFRB_H
#define LOGOFRB_H

#include <QWidget>
#include <QSettings>

class logofrb : public QWidget
{
    Q_OBJECT
public:
    explicit logofrb(QWidget *parent = nullptr);

public slots:
    void on_logo(void);
    void off_logo(void);
    void setLogo(bool);

private:
    virtual void paintEvent(QPaintEvent*);
    QString pathLogo;
    QSettings* settings;

};

#endif // LOGOFRB_H

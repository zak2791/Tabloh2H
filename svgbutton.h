#ifndef SVGBUTTON_H
#define SVGBUTTON_H


#include "qpushbutton.h"

#include "qsvgwidget.h"
#include <QEvent>

class SvgButton : public QPushButton
{
private:
    QSvgWidget w;
    QString svgPathPlay;
    QString svgPathStop;

public:
    SvgButton(QString, QString, QWidget* parent = nullptr);


    void setPlayState(bool);

    virtual void changeEvent(QEvent*);
};

#endif // SVGBUTTON_H

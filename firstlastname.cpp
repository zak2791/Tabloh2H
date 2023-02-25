#include "firstlastname.h"

#include <QPainter>

FirstLastName::FirstLastName(QString _cl,
                             QString _tx,
                             int _wt,
                             QString fn,
                             QWidget *parent)  :
    Fam(_cl, _tx, _wt, fn, parent)
{

}

void FirstLastName::paintEvent(QPaintEvent * ){
    fm.setPixelSize(height() / 1.8);
    fm.setWeight(wt); //63
    setFont(fm);

    QPainter pn;
    pn.begin(this);

    QFontMetrics p(font()); // .width(tx);

    QList<QString> sList = tx.split(" ");
    QString LastName = sList.at(0);
    QString FirstName;
    if(sList.count() > 1)
        FirstName = sList.at(1);
    else
        FirstName = "";

    if(p.width(LastName) >= width())
        pn.drawText(0, height() * 0.45, LastName);
    else
        if(align == 0)
            pn.drawText((width() - p.width(LastName)) / 2, height() * 0.45, LastName);
        else if(align == 1)
            pn.drawText(0, height() * 0.45, LastName);
        else
            pn.drawText(width() - p.width(LastName), height() * 0.45, LastName);

    if(p.width(FirstName) >= width())
        pn.drawText(0, height() * 0.9, FirstName);
    else
        if(align == 0)
            pn.drawText((width() - p.width(FirstName)) / 2, height() * 0.9, FirstName);
        else if(align == 1)
            pn.drawText(0, height() * 0.9, FirstName);
        else
            pn.drawText(width() - p.width(FirstName), height() * 0.9, FirstName);

    //if (align == 0 || align == 1 || p.width(tx) >= width()) {
        QLinearGradient gr(width() * 0.85, 0, width(), 0);
        gr.setColorAt(0, QColor(0, 0, 0, 0));
        gr.setColorAt(1, QColor(0, 0, 0, 255));
        pn.setBrush(gr);
    //}
    pn.setPen(Qt::NoPen);
    pn.drawRect(0, 0, width(), height());

    pn.end();

}

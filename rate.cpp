#include <QtCore>
#include <QMouseEvent>
#include "rate.h"
#include <QMessageBox>

Rate::Rate(QWidget *parent):

QLabel(parent)
{
    rate = 0;

    setStyleSheet("QLabel{border-radius: 30px; "
                         "border-width: 2px; "
                         "border-style: solid; "
                         "border-color:white; "
                         "background-color: black; "
                         "color: white;}");

    setAutoFillBackground(true);
    setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    largeFont = QFont();
    smallFont = QFont();

};


void Rate::plus() {
    if (rate < 99) {
        rate++;
        if (rate == 10) setFont(smallFont);
        setText(QString::number(rate));
        emit sigRate(rate);
    }
}

void Rate::minus() {
    if (rate > 0) {
        rate--;
        if (rate == 9)
            setFont(largeFont);
        setText(QString::number(rate));
        emit sigRate(rate);
    }
}

void Rate::sbros() {
    setRate(0);
    emit sigRate(rate);
}

void Rate::mousePressEvent(QMouseEvent *  pe) {
    if (pe->buttons() == Qt::LeftButton)
        plus();
    else if (pe->buttons() == Qt::RightButton)
        minus();
}

void Rate::setRate(int r) {
    rate = r;

    if (rate < 10) setFont(largeFont);
    else setFont(smallFont);

    setText(QString::number(rate));
    emit sigRate(rate);
}

void Rate::resizeEvent(QResizeEvent * ){
    largeFont.setWeight(50);
    largeFont.setPixelSize(height() * 2);
    QFontMetrics fm = QFontMetrics(largeFont);
    smallFont.setWeight(50);
    smallFont.setPixelSize(height() * 2);
    int fontSize = height();

    fontSize = height() * 1.05;
    while (true) {
        smallFont.setPixelSize(fontSize);
        setFont(smallFont);
        fm = QFontMetrics(smallFont);
        if (fm.horizontalAdvance("00") < width())
            break;
        if (fontSize > 20)
            fontSize -= 20;
        else
            break;
    }

    fontSize = height() * 1.05;
    while (true) {
        largeFont.setPixelSize(fontSize);
        setFont(largeFont);
        fm = QFontMetrics(largeFont);
        if (fm.horizontalAdvance("0") < width() )
            break;
        if (fontSize > 20)
            fontSize -= 20;
        else
            break;
    }

    if (rate > 10) setFont(smallFont);

    setText(QString::number(rate));
}

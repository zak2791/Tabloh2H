#include "endtime.h"
#include <QDebug>
#include <QApplication>

////////////////////////////////////////////////////////
/// Эффект красного экрана при окончании времени боя ///
///                                                  ///
////////////////////////////////////////////////////////
EndTime::EndTime(QWidget *parent) : QLabel(parent)
{
    transparent = 255;

    setStyleSheet("background-color: rgba(255, 0, 0, 0);");
}

void EndTime::startProcess(){
    //finishTransparent = 0;
    id_timer = startTimer(50);   
}

//void EndTime::startTechProcess(){
//    finishTransparent = 150;
//    id_timer = startTimer(50);
//}

void EndTime::timerEvent(QTimerEvent*){
    if(transparent > 0){
        transparent -= 5;
        QString style("background-color: rgba(255, 0, 0, %1);");
        style = style.arg(QString::number(transparent));
        setStyleSheet(style);
        //qDebug()<<style;
    }else{
        killTimer(id_timer);
        transparent = 255;
        QString style("background-color: rgba(255, 0, 0, 0);");
        //style = style.arg(QString::number(finishTransparent));
        setStyleSheet(style);
    }

}

//void EndTime::sbros(){
//    setStyleSheet("background-color: rgba(255, 0, 0, 0);");
//}

void EndTime::mousePressEvent(QMouseEvent* e){
    QMouseEvent* eventCopy = new QMouseEvent(*e);
            QApplication::instance()->postEvent(this->parent(), eventCopy);
            //...but this time, ignore it
            setAttribute(Qt::WA_TransparentForMouseEvents, true);
            //QTimer::singleSlot(1, this, SLOT(resetClickTransparency()));
            //don't propagate original event any further
            e->accept();
}

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
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
}

void EndTime::startProcess(){
    setStyleSheet("background-color: rgba(255, 0, 0, 255);");
    id_timer = startTimer(50);   
}

void EndTime::timerEvent(QTimerEvent*){
    if(transparent > 0){
        transparent -= 5;
        QString style("background-color: rgba(255, 0, 0, %1);");
        style = style.arg(QString::number(transparent));
        setStyleSheet(style);
    }else{
        killTimer(id_timer);
        transparent = 255;
        QString style("background-color: rgba(255, 0, 0, 0);");
        setStyleSheet(style);
    }

}


// void EndTime::mousePressEvent(QMouseEvent* e){
//     // QMouseEvent* eventCopy = new QMouseEvent(*e);//*e);
//     // QMouseEvent* qm2 = new QMouseEvent(QEvent::MouseButtonPress, QPoint(5,5), Qt::LeftButton , Qt::LeftButton, Qt::NoModifier);
//     // QMouseEvent* qm3 = new QMouseEvent(e->type(),
//     //                                    e->globalPosition(),
//     //                                    e->button(),
//     //                                    e->buttons(),
//     //                                    e->modifiers(),
//     //                                    e->device()
//     //                                    );

//     //QApplication::instance()->postEvent(this->parent(), e);
//     //...but this time, ignore it
//     setAttribute(Qt::WA_TransparentForMouseEvents, true);
//     //QTimer::singleSlot(1, this, SLOT(resetClickTransparency()));
//     //don't propagate original event any further
//     e->ignore();
// }

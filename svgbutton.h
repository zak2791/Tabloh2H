#ifndef SVGBUTTON_H
#define SVGBUTTON_H

#include "qboxlayout.h"
#include "qpushbutton.h"
#include "qsvgrenderer.h"
#include "qsvgwidget.h"
#include <QEvent>

class SvgButton : public QPushButton
{
private:
    QSvgWidget *w;
    QString svgPathPlay;
    QString svgPathStop;

public:
    SvgButton(QString pPlay, QString pStop, QWidget* parent = nullptr) : QPushButton(parent)
    {
        svgPathPlay = pPlay;
        svgPathStop = pStop;
        //setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
        w = new QSvgWidget(svgPathPlay,this);
        w->renderer()->setAspectRatioMode(Qt::KeepAspectRatio);

        QHBoxLayout* box = new QHBoxLayout(this);
        box->setContentsMargins(0,0,0,0);
        setLayout( new QHBoxLayout(this) );
        layout()->addWidget(w);
    }

    void setPlayState(bool state){
        //qDebug()<<"setState"<<state;
        if(state){
            w->load(svgPathPlay);
            w->renderer()->setAspectRatioMode(Qt::KeepAspectRatio);
        }
        else {
            w->load(svgPathStop);
            w->renderer()->setAspectRatioMode(Qt::KeepAspectRatio);
        }
    }

    virtual void changeEvent(QEvent* e){
        if(e->type() == QEvent::EnabledChange){
            if(isEnabled()){
                w->load(svgPathPlay);
                w->renderer()->setAspectRatioMode(Qt::KeepAspectRatio);
            }
            else{
                w->load(svgPathStop);
                w->renderer()->setAspectRatioMode(Qt::KeepAspectRatio);
            }
        }
    }

};

#endif // SVGBUTTON_H

#include "svgbutton.h"
#include "qsvgrenderer.h"
#include "qboxlayout.h"

SvgButton::SvgButton(QString pPlay, QString pStop, QWidget* parent) : QPushButton(parent){
    svgPathPlay = pPlay;
    svgPathStop = pStop;
    //setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
    //w = new QSvgWidget(svgPathPlay);
    w.load(svgPathPlay);
    w.renderer()->setAspectRatioMode(Qt::KeepAspectRatio);

    QHBoxLayout* box = new QHBoxLayout(this);
    box->setContentsMargins(0,0,0,0);
    setLayout(box);
    layout()->addWidget(&w);
}

void SvgButton::setPlayState(bool state){
    //qDebug()<<"setState"<<state;
    if(state){
        w.load(svgPathPlay);
        w.renderer()->setAspectRatioMode(Qt::KeepAspectRatio);
    }
    else {
        w.load(svgPathStop);
        w.renderer()->setAspectRatioMode(Qt::KeepAspectRatio);
    }
}

void SvgButton::changeEvent(QEvent* e){
    if(e->type() == QEvent::EnabledChange){
        if(isEnabled()){
            w.load(svgPathPlay);
            w.renderer()->setAspectRatioMode(Qt::KeepAspectRatio);
        }
        else{
            w.load(svgPathStop);
            w.renderer()->setAspectRatioMode(Qt::KeepAspectRatio);
        }
    }
}

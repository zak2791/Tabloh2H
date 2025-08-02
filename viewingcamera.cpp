#include "viewingcamera.h"

ViewingCamera::ViewingCamera(QWidget *parent) : QLabel{parent} {
    indicator = new QLabel(this);
    indicator->setStyleSheet("QLabel{ background-color: rgba(255, 0, 0, 200)}");
    fps = 30.0;
    droppedFrames = 5.0;
}

void ViewingCamera::setDroppedFrames(int value){
    droppedFrames = value;
    int w = width();
    int h = height();
    int w_indicator;
    if(fps > 0)
        w_indicator = droppedFrames / (fps / 2) * (w - 2);
    else
        w_indicator = 0;
    int h_indicator = h / 5;
    w_indicator = w_indicator > w - 2 ? w -2 : w_indicator;
    indicator->setGeometry(2,  h - h_indicator - 2, w_indicator, h_indicator);
}


#ifndef VIEWINGCAMERA_H
#define VIEWINGCAMERA_H

#include "qlabel.h"

class ViewingCamera : public QLabel
{
public:
    ViewingCamera(QWidget *parent = nullptr);
    void setFps(int value){fps = value;}
    void setDroppedFrames(int);

private:
    QLabel* indicator;
    qreal fps;
    qreal droppedFrames;

};

#endif // VIEWINGCAMERA_H

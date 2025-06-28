#ifndef PLAYERTV_H
#define PLAYERTV_H

#include "qvideoframe.h"
#include "qvideoframeinput.h"
#include "qwidget.h"

class PlayerTv : public QWidget
{
public:
    PlayerTv(QWidget *parent = nullptr);
    //QMediaPlayer* player;

private:
    QVideoFrameInput* videoInput;
    bool readySend = true;

public slots:
    void playVideoFrame(QVideoFrame);

};

#endif // PLAYERTV_H

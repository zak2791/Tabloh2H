#ifndef PLAYERTV_H
#define PLAYERTV_H

#include "qgraphicsvideoitem.h"
#include "qgraphicsview.h"
#include "qvideoframe.h"
#include "qvideoframeinput.h"
#include "qwidget.h"

class PlayerTv : public QWidget
{
public:
    PlayerTv(QWidget *parent = nullptr);

private:
    QVideoFrameInput* videoInput;
    bool readySend = true;

    QGraphicsVideoItem videoOutput;
    QGraphicsView* view;

    virtual void resizeEvent(QResizeEvent*) override;

public slots:
    void playVideoFrame(QVideoFrame);
    void setZoom(QTransform, QPointF);

};

#endif // PLAYERTV_H

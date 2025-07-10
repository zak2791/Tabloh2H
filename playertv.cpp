#include "playertv.h"
#include "qboxlayout.h"
#include "qmediacapturesession.h"

PlayerTv::PlayerTv(QWidget *parent) : QWidget{parent}
{
    view = new QGraphicsView;
    view->setBackgroundBrush(QBrush("gray"));
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setScene(new QGraphicsScene);
    view->scene()->addItem(&videoOutput);

    QVBoxLayout* vBox = new QVBoxLayout(this);
    vBox->addWidget(view);
    setLayout(vBox);

    videoInput = new QVideoFrameInput(this);
    QMediaCaptureSession* session = new QMediaCaptureSession(this);
    session->setVideoOutput(&videoOutput);
    session->setVideoFrameInput(videoInput);

    connect(videoInput, &QVideoFrameInput::readyToSendVideoFrame, this, [this](){readySend = true;});

}

void PlayerTv::resizeEvent(QResizeEvent *)
{
    videoOutput.setSize(view->size());
    view->scene()->setSceneRect(view->rect());
}

void PlayerTv::playVideoFrame(QVideoFrame frame)
{
    if(readySend){
        videoInput->sendVideoFrame(frame);
        readySend = false;
    }
}

void PlayerTv::setZoom(QTransform transform, QPointF anchor)
{
    QPoint pos;
    pos.setX((0.5 - anchor.x()) * view->width());
    pos.setY((0.5 - anchor.y()) * view->height());
    view->setTransform(transform);
    view->centerOn(pos);
}

#include "playertv.h"
#include "qboxlayout.h"
#include "qmediacapturesession.h"
#include "qvideowidget.h"

PlayerTv::PlayerTv(QWidget *parent) : QWidget{parent}
{
    QVideoWidget* videoOutput = new QVideoWidget;
    QVBoxLayout* vBox = new QVBoxLayout(this);
    vBox->addWidget(videoOutput);
    setLayout(vBox);

    videoInput = new QVideoFrameInput(this);
    QMediaCaptureSession* session = new QMediaCaptureSession(this);
    session->setVideoOutput(videoOutput);
    session->setVideoFrameInput(videoInput);

    connect(videoInput, &QVideoFrameInput::readyToSendVideoFrame, this, [this](){readySend = true;});

}

void PlayerTv::playVideoFrame(QVideoFrame frame)
{
    if(readySend){
        videoInput->sendVideoFrame(frame);
        readySend = false;
    }
}

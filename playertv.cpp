#include "playertv.h"
#include "qboxlayout.h"
#include "qvideowidget.h"

PlayerTv::PlayerTv(QWidget *parent) : QWidget{parent}
{
    QVideoWidget* videoOutput = new QVideoWidget;
    player = new QMediaPlayer(this);
    player->setVideoOutput(videoOutput);
    QVBoxLayout* vBox = new QVBoxLayout(this);
    vBox->addWidget(videoOutput);
    setLayout(vBox);

    connect(player, &QMediaPlayer::sourceChanged, [](QUrl url){
        qDebug()<<url;
    });

}

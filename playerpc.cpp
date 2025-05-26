#include "playerpc.h"
#include "qboxlayout.h"
#include "qbuttongroup.h"
#include "qframe.h"
#include "qlabel.h"
#include "qpushbutton.h"
#include <QAudioOutput>
#include <QMediaMetaData>


PlayerPc::PlayerPc(QWidget *parent)
    : QWidget{parent}
{
    QVBoxLayout* vLayout = new QVBoxLayout;
    QHBoxLayout* hLayout = new QHBoxLayout;
    btnPlay = new SvgButton(":/images/play.svg", ":/images/stop.svg");

    btnClose = new SvgButton(":/images/off.svg", ":/images/off.svg");
    btnFrameForward = new SvgButton(":/images/frame_forward.svg", ":/images/frame_forward.svg");
    btnFrameBack = new SvgButton(":/images/frame_backward.svg", ":/images/frame_backward.svg");
    videoOutput = new QVideoWidget;

    rbCam1 = new QRadioButton("1");
    rbCam1->setChecked(true);
    rbCam2 = new QRadioButton("2");
    rbCam3 = new QRadioButton("3");

    QButtonGroup* groupVideo = new QButtonGroup(this);
    groupVideo->setExclusive(true);
    groupVideo->addButton(rbCam1);
    groupVideo->addButton(rbCam2);
    groupVideo->addButton(rbCam3);

    rbSound1 = new QRadioButton("1");
    rbSound1->setChecked(true);
    rbSound2 = new QRadioButton("2");
    rbSound3 = new QRadioButton("3");

    sliderPosition = new QSlider;
    sliderPosition->setOrientation(Qt::Horizontal);

    sliderVolume = new QSlider;
    sliderVolume->setOrientation(Qt::Horizontal);
    sliderVolume->setMaximumWidth(width() / 8);
    sliderVolume->setRange(0, 100);
    sliderVolume->setValue(100);

    playbackRate = new QDoubleSpinBox(this);
    playbackRate->setMaximum(4.0);
    playbackRate->setMinimum(0.5);
    playbackRate->setDecimals(1);
    playbackRate->setSingleStep(0.1);
    playbackRate->setPrefix("x");
    playbackRate->setValue(1.0);

    QFrame* vLine1 = new QFrame;
    vLine1->setFrameShape(QFrame::VLine);
    QFrame* vLine2 = new QFrame;
    vLine2->setFrameShape(QFrame::VLine);
    QFrame* vLine3 = new QFrame;
    vLine3->setFrameShape(QFrame::VLine);
    QFrame* vLine4 = new QFrame;
    vLine4->setFrameShape(QFrame::VLine);

    hLayout->addWidget(btnPlay, 1);
    hLayout->addWidget(btnFrameBack, 1);
    hLayout->addWidget(btnFrameForward, 1);
    hLayout->addWidget(vLine1);
    hLayout->addWidget(new QLabel("Камера"));
    hLayout->addWidget(rbCam1);
    hLayout->addWidget(rbCam2);
    hLayout->addWidget(rbCam3);
    hLayout->addWidget(vLine2);
    hLayout->addWidget(new QLabel("Звук"));
    hLayout->addWidget(rbSound1);
    hLayout->addWidget(rbSound2);
    hLayout->addWidget(rbSound3);
    hLayout->addWidget(vLine3);
    hLayout->addWidget(new QLabel("Громкость"));
    hLayout->addWidget(sliderVolume);
    hLayout->addWidget(vLine4);
    hLayout->addWidget(new QLabel("Скорость"));
    hLayout->addWidget(playbackRate);

    hLayout->addStretch(8);
    hLayout->addWidget(btnClose, 1);

    vLayout->addWidget(videoOutput);
    vLayout->addWidget(sliderPosition);
    vLayout->addLayout(hLayout);
    setLayout(vLayout);
    setGeometry(0, 0, 500, 300);
    setWindowModality(Qt::ApplicationModal);

    player = new QMediaPlayer(this);
    player->setVideoOutput(videoOutput);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);

    connect(player, &QMediaPlayer::positionChanged, sliderPosition, &QSlider::setSliderPosition);
    connect(sliderPosition, &QSlider::valueChanged, player, &QMediaPlayer::setPosition);

    connect(player, &QMediaPlayer::tracksChanged, [this](){
        qDebug()<<"player->activeAudioTrack() = "<<player->activeAudioTrack();
    });
    connect(player, &QMediaPlayer::metaDataChanged, this, &PlayerPc::metaDataChanged);

    connect(btnPlay, &QPushButton::clicked, player, [this](){
        if(player->isPlaying()){
            player->pause();
            sliderPosition->setEnabled(true);
            sliderPosition->setTracking(true);
        }else{
            sliderPosition->setTracking(false);
            sliderPosition->setEnabled(false);
            player->play();
        }
    });

    connect(btnFrameBack, &QPushButton::clicked, this, [this](){
        player->setPosition(player->position() - 33);
    });
    connect(btnFrameForward, &QPushButton::clicked, this, [this](){
        player->setPosition(player->position() + 33);
    });

    connect(rbCam1, &QRadioButton::clicked, this, &PlayerPc::selectVideoTrack);
    connect(rbCam2, &QRadioButton::clicked, this, &PlayerPc::selectVideoTrack);
    connect(rbCam3, &QRadioButton::clicked, this, &PlayerPc::selectVideoTrack);

    connect(rbSound1, &QRadioButton::clicked, this, &PlayerPc::selectAudioTrack);
    connect(rbSound2, &QRadioButton::clicked, this, &PlayerPc::selectAudioTrack);
    connect(rbSound3, &QRadioButton::clicked, this, &PlayerPc::selectAudioTrack);

    connect(sliderVolume, &QSlider::valueChanged, audioOutput, &QAudioOutput::setVolume);
    connect(playbackRate, &QDoubleSpinBox::valueChanged, player, &QMediaPlayer::setPlaybackRate);
    connect(player, &QMediaPlayer::playbackStateChanged, this, &PlayerPc::playState);

    connect(btnClose, &QPushButton::clicked, this, &QWidget::close);

    QList<QWidget *> widgets = findChildren<QWidget *>();
    foreach (auto each, widgets) {
        each->setFocusPolicy(Qt::NoFocus);
    }

}

void PlayerPc::setMediaUrl(QString url)
{
    player->setSource(QUrl::fromLocalFile(url));
    player->pause();
    playerTv->pause();
    audioOutput->setVolume(sliderVolume->value());
    player->setPlaybackRate(playbackRate->value());
}

void PlayerPc::setTvPlayer(QMediaPlayer* p)
{
    playerTv = p;
    connect(player, &QMediaPlayer::sourceChanged, playerTv, &QMediaPlayer::setSource);
    connect(rbCam1, &QRadioButton::clicked, this, [this](bool checked){
        if(checked){
            playerTv->setActiveVideoTrack(0);
            playerTv->setPosition(player->position());
        }
    });
    connect(rbCam2, &QRadioButton::clicked, this, [this](bool checked){
        if(checked){
            playerTv->setActiveVideoTrack(1);
            playerTv->setPosition(player->position());
        }
    });
    connect(rbCam3, &QRadioButton::clicked, this, [this](bool checked){
        if(checked){
            playerTv->setActiveVideoTrack(2);
            playerTv->setPosition(player->position());
        }
    });
    connect(player, &QMediaPlayer::playbackRateChanged, playerTv, &QMediaPlayer::setPlaybackRate);

}

void PlayerPc::closeEvent(QCloseEvent*)
{
    player->stop();
    emit sigClose();
}

void PlayerPc::keyPressEvent(QKeyEvent* e)
{
    if(e->key() == Qt::Key_Right)
        player->setPosition(player->position() + 33);
    if(e->key() == Qt::Key_Left)
        player->setPosition(player->position() - 33);

}

void PlayerPc::playState(QMediaPlayer::PlaybackState state)
{
    if(state == QMediaPlayer::PlayingState){
        disconnect(player, &QMediaPlayer::positionChanged, playerTv, &QMediaPlayer::setPosition);
        playerTv->play();
        btnPlay->setPlayState(false);
    }
    else if(state == QMediaPlayer::StoppedState){
        playerTv->stop();
        btnPlay->setPlayState(true);
    }
    else if(state == QMediaPlayer::PausedState){
        playerTv->pause();
        btnPlay->setPlayState(true);
        connect(player, &QMediaPlayer::positionChanged, playerTv, &QMediaPlayer::setPosition);
    }
}

void PlayerPc::selectVideoTrack(bool checked)
{
    if(!checked)
        return;
    if(rbCam1->isChecked())
        player->setActiveVideoTrack(0);
    else if(rbCam2->isChecked())
        player->setActiveVideoTrack(1);
    else
        player->setActiveVideoTrack(2);
    player->setPosition(player->position());
}

void PlayerPc::selectAudioTrack(bool checked)
{
    if(!checked)
        return;
    qDebug()<<checked<<player->audioTracks().size()<<player->activeAudioTrack();
    if(rbSound1->isChecked())
        player->setActiveAudioTrack(0);
    else if(rbSound2->isChecked())
        player->setActiveAudioTrack(1);
    else
        player->setActiveAudioTrack(2);
}

void PlayerPc::metaDataChanged()
{
    QMediaMetaData data = player->metaData();
    int duration = data.value(QMediaMetaData::Duration).toInt();
    sliderPosition->setRange(0, duration);
    int countVideos = player->videoTracks().size();
    int countSounds = player->audioTracks().size();
    if(countVideos == 1){
        rbCam2->setEnabled(false);
        rbCam3->setEnabled(false);
    }
    else if(countVideos == 2){
        rbCam2->setEnabled(true);
        rbCam3->setEnabled(false);
    }
    else{
        rbCam2->setEnabled(true);
        rbCam3->setEnabled(true);
    }
    rbCam1->setChecked(true);

    if(countSounds == 1){
        rbSound1->setEnabled(true);
        rbSound2->setEnabled(false);
        rbSound3->setEnabled(false);
    }
    else if(countSounds == 2){
        rbSound1->setEnabled(true);
        rbSound2->setEnabled(true);
        rbSound3->setEnabled(false);
    }
    else if(countSounds == 3){
        rbSound1->setEnabled(true);
        rbSound2->setEnabled(true);
        rbSound3->setEnabled(true);
    }
    else{
        rbSound1->setEnabled(false);
        rbSound2->setEnabled(false);
        rbSound3->setEnabled(false);
    }

    videoOutput->update();
    //player->setPosition(duration / 2);
    //player->play();
    //player->pause();
    //sliderPosition->setSliderPosition(0);

}

#include "playerpc.h"
#include "playertv.h"
#include "qboxlayout.h"
#include "qcheckbox.h"
#include "qframe.h"
#include "qgraphicsview.h"
#include "qlabel.h"
#include "qpushbutton.h"
#include <QVideoSink>
#include <QAudioOutput>
#include <QMediaMetaData>
#include <QGraphicsSceneWheelEvent>

PlayerPc::PlayerPc(QWidget *parent)
    : QWidget{parent}
{
    QVBoxLayout* vLayout = new QVBoxLayout;
    QHBoxLayout* hLayout = new QHBoxLayout;
    btnPlay = new SvgButton(":/images/play.svg", ":/images/stop.svg");

    btnClose = new SvgButton(":/images/off.svg", ":/images/off.svg");
    btnFrameForward = new SvgButton(":/images/frame_forward.svg", ":/images/frame_forward.svg");
    btnFrameBack = new SvgButton(":/images/frame_backward.svg", ":/images/frame_backward.svg");

    view = new QGraphicsView;
    view->setBackgroundBrush(QBrush("gray"));
    view->setScene(new QGraphicsScene);
    view->scene()->addItem(&videoOutput);
    view->scene()->installEventFilter( this );
    view->setMouseTracking(true);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    selectVideo = new QComboBox;
    selectVideo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    connect(selectVideo, &QComboBox::activated, this, &PlayerPc::selectVideoStream);

    selectAudio = new QComboBox;
    connect(selectAudio, &QComboBox::activated, this, &PlayerPc::selectAudioStream);
    selectAudio->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    sliderPosition = new QSlider;
    sliderPosition->setOrientation(Qt::Horizontal);

    sliderVolume = new QSlider;
    sliderVolume->setOrientation(Qt::Horizontal);
    sliderVolume->setMaximumWidth(width() / 8);
    sliderVolume->setRange(0, 100);
    sliderVolume->setValue(100);

    playbackRate = new QDoubleSpinBox(this);
    playbackRate->setMaximum(2.0);
    playbackRate->setMinimum(0.2);
    playbackRate->setDecimals(1);
    playbackRate->setSingleStep(0.2);
    playbackRate->setPrefix("x");
    playbackRate->setValue(1.0);

    cbRepeat = new QCheckBox(this);
    sbRepeat = new QSpinBox(this);

    connect(cbRepeat, &QCheckBox::clicked, this, [this](bool checked){
        repeatPos = player->position();
        repeatTime = sbRepeat->value();
        repeat = checked;
    });

    sbRepeat->setRange(1, 10);
    sbRepeat->setSuffix(" c");
    connect(sbRepeat, &QSpinBox::valueChanged, this, [this](int time){
        repeatTime = time;
    });

    QFrame* vLine1 = new QFrame;
    vLine1->setFrameShape(QFrame::VLine);
    QFrame* vLine2 = new QFrame;
    vLine2->setFrameShape(QFrame::VLine);
    QFrame* vLine3 = new QFrame;
    vLine3->setFrameShape(QFrame::VLine);
    QFrame* vLine4 = new QFrame;
    vLine4->setFrameShape(QFrame::VLine);
    QFrame* vLine5 = new QFrame;
    vLine5->setFrameShape(QFrame::VLine);

    hLayout->addWidget(btnPlay, 1);
    hLayout->addWidget(btnFrameBack, 1);
    hLayout->addWidget(btnFrameForward, 1);
    hLayout->addWidget(vLine1);
    hLayout->addWidget(new QLabel("Камера"));
    hLayout->addWidget(selectVideo);
    hLayout->addWidget(vLine2);
    hLayout->addWidget(new QLabel("Звуковая дорожка"));
    hLayout->addWidget(selectAudio);
    hLayout->addWidget(vLine3);
    hLayout->addWidget(new QLabel("Громкость"));
    hLayout->addWidget(sliderVolume);
    hLayout->addWidget(vLine4);
    hLayout->addWidget(new QLabel("Скорость"));
    hLayout->addWidget(playbackRate);
    hLayout->addWidget(vLine5);
    hLayout->addWidget(new QLabel("Повтор"));
    hLayout->addWidget(cbRepeat);
    hLayout->addWidget(sbRepeat);

    hLayout->addStretch(8);
    hLayout->addWidget(btnClose, 1);

    vLayout->addWidget(view);
    vLayout->addWidget(sliderPosition);
    vLayout->addLayout(hLayout);
    setLayout(vLayout);
    setGeometry(0, 0, 500, 300);
    setWindowModality(Qt::ApplicationModal);

    player = new QMediaPlayer(this);
    player->setVideoOutput(&videoOutput);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);

    connect(player, &QMediaPlayer::tracksChanged, this, &PlayerPc::tracksChanged);
    connect(player, &QMediaPlayer::positionChanged, sliderPosition, &QSlider::setSliderPosition);
    connect(&timer, &QTimer::timeout, this, [this](){timeoutSliderPosition = true;});
    connect(sliderPosition, &QSlider::valueChanged, this, [this](int pos){
        if(timeoutSliderPosition){
            timeoutSliderPosition = false;
            player->setPosition(pos);
        }
    });
    timer.start(100);
    connect(player, &QMediaPlayer::positionChanged, this, [this](int position){
        if(repeat){
            if(position >= duration - 100){
                int newPos = position - repeatTime * 1000;
                if(newPos <= 0)
                    newPos = 1;
                if(player->playbackState() == QMediaPlayer::PlayingState){
                    player->pause();
                    player->setPosition(newPos);
                    player->play();
                }
                else{
                    player->setPosition(newPos);
                }
            }
            else if(position > repeatPos + repeatTime * 1000 / 2){
                if(player->playbackState() == QMediaPlayer::PlayingState){
                    player->pause();
                    player->setPosition(position - repeatTime * 1000);
                    player->play();
                }
                else{
                    player->setPosition(position - repeatTime * 1000);
                }
            }
        }
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


    connect(sliderVolume, &QSlider::valueChanged, audioOutput, [this](int volume){
        audioOutput->setVolume(volume / 100.0);
    });
    connect(playbackRate, &QDoubleSpinBox::valueChanged, player, &QMediaPlayer::setPlaybackRate);
    connect(player, &QMediaPlayer::playbackStateChanged, this, &PlayerPc::playState);

    connect(btnClose, &QPushButton::clicked, this, &PlayerPc::close);

    QList<QWidget *> widgets = findChildren<QWidget *>();
    foreach (auto each, widgets) {
        each->setFocusPolicy(Qt::NoFocus);
    }

    sinc = videoOutput.videoSink();

}

void PlayerPc::setMediaUrl(QString url)
{
    player->setSource(QUrl::fromLocalFile(url));
    player->pause();
    audioOutput->setVolume(sliderVolume->value());
    player->setPlaybackRate(playbackRate->value());
}

void PlayerPc::setTvPlayer(PlayerTv *p)
{
    playerTv = p;
    connect(sinc, &QVideoSink::videoFrameChanged, playerTv, &PlayerTv::playVideoFrame);
    connect(this, &PlayerPc::sigZoom, playerTv, &PlayerTv::setZoom);

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
    if(e->key() == Qt::Key_Space)
        cbRepeat->click();

}

void PlayerPc::playState(QMediaPlayer::PlaybackState state)
{
    if(state == QMediaPlayer::PlayingState)
        btnPlay->setPlayState(false);
    else if(state == QMediaPlayer::StoppedState)
        btnPlay->setPlayState(true);
    else if(state == QMediaPlayer::PausedState)
        btnPlay->setPlayState(true);
}

void PlayerPc::tracksChanged(){
    selectVideo->clear();
    const auto videoTracks = player->videoTracks();
    selectVideo->addItem(QStringLiteral("No video"), - 1);
    for(int i = 0; i < videoTracks.size(); ++i)
        selectVideo->addItem(QString::number(i + 1), i);
    selectVideo->setCurrentIndex(player->activeVideoTrack() + 1);

    selectAudio->clear();
    const auto audioTracks = player->audioTracks();
    selectAudio->addItem(QStringLiteral("No audio"), - 1);
    for(int i = 0; i < audioTracks.size(); ++i)
        selectAudio->addItem(QString::number(i + 1), i);
    selectAudio->setCurrentIndex(player->activeAudioTrack() + 1);
}

void PlayerPc::selectVideoStream()
{
    int stream = selectVideo->currentData().toInt();
    player->setActiveVideoTrack(stream);
    if(player->playbackState() == QMediaPlayer::PausedState){
        player->setPosition(player->position());
    }
}

void PlayerPc::selectAudioStream()
{
    int stream = selectAudio->currentData().toInt();
    player->setActiveAudioTrack(stream);
}


void PlayerPc::metaDataChanged()
{
    QMediaMetaData data = player->metaData();
    duration = data.value(QMediaMetaData::Duration).toInt();
    sliderPosition->setRange(0, duration);

}

void PlayerPc::resizeEvent(QResizeEvent*)
{
    videoOutput.setSize(view->size());
    view->scene()->setSceneRect(view->rect());
}

bool PlayerPc::eventFilter(QObject *o, QEvent *e)
{
    if ( o == view->scene() && e->type() == QEvent::GraphicsSceneWheel )
    {
        view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        QPointF anchor;
        anchor.setX(0.5 - ((qreal)zoomAnchor.x() / view->width()));
        anchor.setY(0.5 - ((qreal)zoomAnchor.y() / view->height()));

        if(static_cast<QGraphicsSceneWheelEvent *>(e)->delta() > 0){
            if(zoomCount < 4){
                view->scale(scaleFactor, scaleFactor);
                zoomCount++;
                emit sigZoom(view->transform(), anchor);
            }
        }
        else{
            if(zoomCount > 1){
                view->scale(1 / scaleFactor, 1 / scaleFactor);
                zoomCount--;
                emit sigZoom(view->transform(), anchor);
            }else if(zoomCount == 1){
                view->setTransform(QTransform());
                zoomCount--;
                emit sigZoom(QTransform(), anchor);
            }
        }
        e->accept();
        return true;
    }
    else if(o == view->scene() && e->type() == QEvent::GraphicsSceneMouseMove){
        if (auto *mouseEvent = dynamic_cast<QGraphicsSceneMouseEvent *>(e)) {
            zoomAnchor = mouseEvent->scenePos().toPoint();
        }
        return true;
    }
    return false;
}

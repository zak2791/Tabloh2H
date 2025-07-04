#ifndef PLAYERPC_H
#define PLAYERPC_H

#include "playertv.h"
#include "qcheckbox.h"
#include "qcombobox.h"
#include "qmediacapturesession.h"
#include "qmediaplayer.h"
#include "qslider.h"
#include "qspinbox.h"
#include "qvideoframe.h"
#include "qvideowidget.h"
#include "svgbutton.h"
#include <QWidget>
#include <QKeyEvent>
#include <QVideoSink>

class PlayerPc : public QWidget
{
    Q_OBJECT
public:
    explicit PlayerPc(QWidget *parent = nullptr);
    void setMediaUrl(QString);
    void setTvPlayer(PlayerTv*);

private:
    virtual void closeEvent(QCloseEvent*);
    virtual void keyPressEvent(QKeyEvent*);
    QMediaPlayer* player;
    PlayerTv* playerTv;
    QSlider* sliderPosition;
    QSlider* sliderVolume;
    QAudioOutput* audioOutput;
    QDoubleSpinBox* playbackRate;

    SvgButton* btnPlay;
    SvgButton* btnClose;
    SvgButton* btnFrameForward;
    SvgButton* btnFrameBack;
    QVideoWidget* videoOutput;
    QCheckBox* cbRepeat;
    QSpinBox* sbRepeat;
    int repeatPos;
    int repeatTime;
    int duration;
    //int fps;
    bool repeat = false;

    QComboBox* selectVideo;
    QComboBox* selectAudio;
    QVideoSink* sinc;

    void playState(QMediaPlayer::PlaybackState);

private slots:
    void selectVideoStream(void);
    void selectAudioStream(void);
    void metaDataChanged(void);
    void tracksChanged(void);

    //void statusChanged(QMediaPlayer::MediaStatus status);

signals:
    void sigClose(void);
    void sigFrame(QVideoFrame);

};

#endif // PLAYERPC_H

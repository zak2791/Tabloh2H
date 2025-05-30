#ifndef PLAYERPC_H
#define PLAYERPC_H

#include "qcheckbox.h"
#include "qmediaplayer.h"
#include "qradiobutton.h"
#include "qslider.h"
#include "qspinbox.h"
#include "qvideowidget.h"
#include "svgbutton.h"
#include <QWidget>
#include <QKeyEvent>


class PlayerPc : public QWidget
{
    Q_OBJECT
public:
    explicit PlayerPc(QWidget *parent = nullptr);
    void setMediaUrl(QString);
    void setTvPlayer(QMediaPlayer*);

private:
    virtual void closeEvent(QCloseEvent*);
    virtual void keyPressEvent(QKeyEvent*);
    QMediaPlayer* player;
    QMediaPlayer* playerTv;
    QSlider* sliderPosition;
    QSlider* sliderVolume;
    QAudioOutput* audioOutput;
    QDoubleSpinBox* playbackRate;
    QRadioButton* rbCam1;
    QRadioButton* rbCam2;
    QRadioButton* rbCam3;
    QRadioButton* rbSound1;
    QRadioButton* rbSound2;
    QRadioButton* rbSound3;
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

    void playState(QMediaPlayer::PlaybackState);

private slots:
    void selectVideoTrack(bool);
    void selectAudioTrack(bool);
    void metaDataChanged(void);

signals:
    void sigClose(void);

};

#endif // PLAYERPC_H

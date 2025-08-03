#ifndef PLAYERPC_H
#define PLAYERPC_H

#include "playertv.h"
#include "qcheckbox.h"
#include "qcombobox.h"
#include "qgraphicsview.h"
#include "qmediacapturesession.h"
#include "qmediaplayer.h"
#include "qslider.h"
#include "qspinbox.h"
#include "qtimer.h"
#include "qvideoframe.h"
//#include "qvideowidget.h"
#include "svgbutton.h"
#include <QWidget>
#include <QKeyEvent>
#include <QVideoSink>
#include <QGraphicsVideoItem>

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
    //QVideoWidget* videoOutput;
    QGraphicsView* view;
    QGraphicsVideoItem videoOutput;

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

    float scaleFactor = 1.5;
    int zoomCount = 0;
    QPoint zoomAnchor;

    void playState(QMediaPlayer::PlaybackState);
    void resizeEvent(QResizeEvent*) override;
    bool eventFilter(QObject *o, QEvent *e) override;
    bool timeoutSliderPosition;
    QTimer timer;

private slots:
    void selectVideoStream(void);
    void selectAudioStream(void);
    void metaDataChanged(void);
    void tracksChanged(void);

    //void statusChanged(QMediaPlayer::MediaStatus status);

signals:
    void sigClose(void);
    void sigFrame(QVideoFrame);
    void sigZoom(QTransform, QPointF);

};

#endif // PLAYERPC_H

#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QImage>

#include <cinttypes>

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavutil/avutil.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/timestamp.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavdevice/avdevice.h>
}

class Player : public QObject
{
    Q_OBJECT

    QImage avFrame2QImage(AVFrame*);
    int flag_seek = 0;
    bool flag_play = false;
    bool flag_one_next_frame = false;
    bool process = false;
    int oneFrameDuration;
    QList<int> arrOneFrameDuration;
    QList<QImage>* bufImage;
    int currentImage = 0;
    QString videoFile;
    int currStream = 0;
    int video_streams;

public:
    explicit Player(QString file = "", QObject *parent = nullptr);
    ~Player();
    void seek(int);
    void turnOff(void);
    void turnPlay(void);

    static QStringList getListWebCams(void);
    static QList<QList<int> > getListParamWebCam(QString);
    static QStringList getListSoundDevices(void);

public slots:
    void Play(void);
    void nextFrame(void);
    void previewFrame(void);
    void setCamera(int);

signals:
    void sigImage(QImage);
    void sigParam(int, int, int);
    void sigFrame(int);
    void sigBuffer(int, int);
    void sigStartStopPlay(bool);
    void sigTime(QString);
    void sigClose(void);
    void sigCountStreams(int);

};

#endif // PLAYER_H

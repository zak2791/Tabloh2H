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
    QList<QImage>* bufImage;
    int currentImage = 0;
    QString videoFile;

public:
    explicit Player(QString file = "", QObject *parent = nullptr);
    void seek(int);
    void turnOff(void);
    void turnPlay(void);

public slots:
    void Play(void);
    void nextFrame(void);
    void previewFrame(void);

signals:
    void sigImage(QImage);
    void sigParam(int, int, int);
    void sigFrame(int);
    void sigBuffer(int, int);

};

#endif // PLAYER_H

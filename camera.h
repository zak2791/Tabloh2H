#ifndef CAMERA_H
#define CAMERA_H

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

class Camera : public QObject
{
    Q_OBJECT
public:
    explicit Camera(QObject *parent = nullptr);

    void StartRecord(QString);
    void StopRecord(void);
    void TurnOffCamera(void);
    void setUrl(QString);

public slots:
        void TurnOnCamera();

private:
    QImage avFrame2QImage(AVFrame*);

    AVFormatContext *ofmt_ctx;
    const AVOutputFormat *ofmt;
    int flag_record;
    int64_t _pts;
    int64_t _dts;
    bool process;
    QString url;
    QString file;
    int prepareRecord(AVFormatContext*, int*, QString);

signals:
    void sigImage(QImage);
    void finished(void);
};

#endif // WORKWITHVIDEO_H

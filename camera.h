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
    #include <libavutil/dict.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavdevice/avdevice.h>
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

    QStringList getListWebCams(void);
    QList<QList<int> > getListParamWebCam(QString);
    QStringList getListSoundDevices(void);

public slots:
        void TurnOnCamera();
        void onlyKeyFrame(bool);

private:
    QImage avFrame2QImage(AVFrame*);

    //AVInputFormat* inFormat;

    void readProcess(void);

    AVFormatContext *ofmt_ctx;
    const AVOutputFormat *ofmt;
    int flag_record;
    bool flag_start_record;
    int64_t _pts;
    int64_t _dts;
    bool process;
    bool main_process;
    QString url;
    QString file;
    int prepareRecord(AVFormatContext*, int*, QString);
    bool only_key_frame;

signals:
    void sigImage(QImage);
    void finished(void);
};

#endif // WORKWITHVIDEO_H

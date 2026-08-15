#ifndef CAMERAWORKER_H
#define CAMERAWORKER_H


#include "qimage.h"
#include "qobject.h"

#include "qprocess.h"
#include "qtcpserver.h"
#include "qtcpsocket.h"
#include <QTimer>
#include <QQueue>
#include "main.h"

extern "C" {
#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/imgutils.h>
#include <libavutil/hwcontext.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/common.h>
#include <libavutil/frame.h>
#include <libavutil/samplefmt.h>
}

typedef struct FilteringContext {
    AVFilterContext *buffersink_ctx;
    AVFilterContext *buffersrc_ctx;
    AVFilterGraph *filter_graph;

    AVPacket *enc_pkt;
    AVFrame *filtered_frame;
} FilteringContext;

class CameraWorker  : public QObject
{
    Q_OBJECT
public:
    CameraWorker(int,
                 int,
                 int,
                 int,
                 QString,
                 bool sound = false,
                 QString url = "",
                 QObject* parent = nullptr);
    ~CameraWorker();

private:
    AVHWDeviceType hwType;
    int configureError = 0;
    const char* urlVk;
    AVCodecContext* encoderContext = NULL;
    AVFormatContext* contextVk = NULL;
    QImage avFrame2QImage(AVFrame *frame);
    int cameraNumber;

    AVCodecContext* decoderContext = NULL;
    QTcpSocket* socket;
    QTcpSocket* videoSocket;
    QTcpSocket* audioSocket;
    int videoPort;
    int audioPort;
    bool isStream = true;
    bool isSound = true;
    bool errStream = true;
    QString vkUrl = "";
    bool isUpdate;
    bool firstAac = true;
    bool firstAvvc = true;
    FilteringContext *filter_ctx;
    bool enabledFilter = false;

    QTimer* tmrCheckConnection;
    // QTimer* tmrCheckStream;
    // QProcess* procCheckStream;

    bool isConnect;

    AVCodecContext *createDecoderContext(int width, int height, int fps);

    int configure(int, int, int, bool);

    QVector<uint8_t> extraDataAnnexbToAvcc(uint8_t *data, int len);

    int addNewVideoStream(AVFormatContext *context, AVCodecContext *fmtContext, int w, int h, int f, int id, bool isStream);

    int addNewAudioStream(AVFormatContext *context, int id);

    int initFilter(AVCodecContext *dec_ctx, AVCodecContext *enc_ctx, AVPixelFormat fmt);

    int init_filter(FilteringContext *fctx, AVCodecContext *dec_ctx, AVCodecContext *enc_ctx, AVPixelFormat fmt, const char *filter_spec);

    int filter_encode_write_frame(AVFrame *frame);
    void readAudioPacket(void);
    void videoHandler(packet);
    void audioHandler(packet);

public slots:
    void start(void);


private slots:
    void readVideoPacket();
    //void checkStream(void);

signals:
    void sigFrame(QImage);
    void sigVideoPacket(packet);
    void sigSoundPacket(packet);
    void sigIsStream(bool);
    void sigExit(void);

};

#endif // CAMERAWORKER_H

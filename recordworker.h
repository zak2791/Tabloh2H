#ifndef RECORDWORKER_H
#define RECORDWORKER_H

#include "qobject.h"
#include "main.h"
#include <QMap>

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

class RecordWorker : public QObject
{
    Q_OBJECT
public:
    RecordWorker(QString,   //file
                 QMap<int, QList<int>>,      //number of video tracks
                 QList<packet*> p,
                 QObject *parent = nullptr);

    ~RecordWorker();

    QString url;

public slots:
    void start(void);
    void setAvailableCams();
    void packetHandler(packet, int);

private:
    AVFormatContext* outputContext;
    QMap<int, QList<int>> params;
    QMap<int, int> tracks;
    int addNewVideoStream(AVFormatContext *context, QList<int>, int);
    int addNewAudioStream(AVFormatContext *context);
    qlonglong startPtsVideo1 = -1;
    qlonglong startPtsAudio = -1;
    //packet* firstVideo3packet;
    QList<packet*> firstPackets;
};

#endif // RECORDWORKER_H

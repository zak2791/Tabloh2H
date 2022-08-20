#ifndef FFMPEGTHREAD_H
#define FFMPEGTHREAD_H

#include <QThread>
#include <QPixmap>

//#include <unistd.h>
#include <stdio.h>
#include <assert.h>
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

#ifdef av_ts2str
#undef av_ts2str
#include <string>

av_always_inline std::string av_ts2string(int ts) {
    char str[AV_TS_MAX_STRING_SIZE];
    return av_ts_make_string(str, ts);
}
#define av_ts2str(ts) av_ts2string(ts).c_str()
#endif

#ifdef av_ts2timestr
#undef av_ts2timestr

av_always_inline std::string av_ts2timestring(int ts, AVRational *tb) {
    char str[AV_TS_MAX_STRING_SIZE];
    return av_ts_make_time_string(str, ts, tb);
}
#define av_ts2timestr(ts, tb) av_ts2timestring(ts, tb).c_str()
#endif



class FFmpegThread : public QObject
{
    Q_OBJECT

    QString in_file, out_file;

    void start();
    QString Url;
    int cam;

    int count_replay;

    //AVFormatContext* ofmt_ctx;



    bool rec;
    uint64_t ptsShift;
    uint64_t dtsShift;

    bool turn;  //0 - выключить чтение потока, 1 - включить чтение потока.
    bool mode_camera;   //режим работы: 0 - предпросмотр, 1 - 3апись.

    void processPreview(void);
    void processRecord(void);

    int time_replay;

public:
    FFmpegThread(int);


    void stop();

    void setUrl(QString);

    void setTimeReplay(int);
    //int state;  //текущее состояние: 0 - нет чтения с потока, 1 - есть приём потока, 2 - идёт запись.
    QString playFile;

signals:
    void err(void);
    void img(QPixmap);
    void imgView(QImage);
    void imgReplay(QImage, int);
    void sig_frame(AVFrame*);

public slots:
    void turn_on(void);  //включение камеры
    void turn_off(void);  //включение камеры
    void startReplay(void);
    void startRecord(void);
    void stopRecord(void);


};

#endif // FFMPEGTHREAD_H

#include "ffmpegthread.h"
#include <QImage>
#include <QDebug>
#include <QTime>

static void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt, const char *){
    AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;
    //printf("%s: pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
           //tag,
           av_ts2str(pkt->pts);//,
           av_ts2timestr(pkt->pts, time_base);//,
           av_ts2str(pkt->dts),
           av_ts2timestr(pkt->dts, time_base);//,
           av_ts2str(pkt->duration);//,
           av_ts2timestr(pkt->duration, time_base);//,
           //pkt->stream_index);
}

// Conversion function
QImage avFrame2QImage(AVFrame *frame){

    struct SwsContext *img_convert_ctx = NULL;

    img_convert_ctx = sws_getContext(frame->width, frame->height, (AVPixelFormat)frame->format, frame->width, frame->height, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);

    if (img_convert_ctx == nullptr){
        sws_freeContext(img_convert_ctx);
        return QImage();
    }

    QImage img (frame->width, frame->height, QImage::Format_RGB888); // Constructing a QImage for Output
    int outputLinesize [3]; // Construct data required by AVFrame to QImage
    av_image_fill_linesizes(outputLinesize, AV_PIX_FMT_RGB24, frame->width);
    uint8_t *outputDst[]={img.bits()};
    sws_scale(img_convert_ctx, frame->data, frame->linesize, 0, frame->height, outputDst, outputLinesize);

    sws_freeContext(img_convert_ctx);

    return img;
}

FFmpegThread::FFmpegThread(int camera) : QObject(){
    //in_file = in;
    //out_file = out;
    //state = 0;
    turn = 0;
    count_replay = 0;
    cam = camera;
    //ofmt_ctx = NULL;
    //ifmt_ctx = NULL;

    //ofmt = NULL;

    rec = false;
    mode_camera = 0;
    time_replay = 3;
    playFile = QString("");
}

void FFmpegThread::setTimeReplay(int time){
    time_replay = time;
}

void FFmpegThread::startReplay(){
    qDebug()<<"startReplay";
    if(count_replay == 0 && turn)
        count_replay = time_replay * 30;
}

void FFmpegThread::turn_on(){
    turn = 1;
    start();
}


void FFmpegThread::turn_off(){
    turn = 0;
    mode_camera = 0;
}

void FFmpegThread::setUrl(QString sUrl){
    Url = sUrl;
}

void FFmpegThread::startRecord(){
    mode_camera = 1;
}


void FFmpegThread::stopRecord(){
    mode_camera = 0;
    qDebug()<<playFile;
}

void FFmpegThread::start(){
    while(turn){
        while(turn && !mode_camera){
            qDebug()<<"preview";
            processPreview();
        }
        while(turn && mode_camera){
            qDebug()<<"record";
            processRecord();
        }
    }
}

void FFmpegThread::processPreview(){
    AVFormatContext *ifmt_ctx = NULL;

    AVPacket *pkt = NULL;

    const char *url;
    int ret; //i;

    AVCodecContext *pCodecCtx = NULL;
    AVStream *stream;
    const AVCodec *dec;
    bool flag_prewiew = false;
    QByteArray bain = Url.toLocal8Bit();
    url = bain.data();
    pkt = av_packet_alloc();
    if (!pkt) {
        fprintf(stderr, "Could not allocate AVPacket\n");
        goto err;
    }
    while(avformat_open_input(&ifmt_ctx, url, 0, 0) < 0){
        if(turn == 0){
            av_packet_free(&pkt);
            goto err;
        }
    }
    if (avformat_find_stream_info(ifmt_ctx, 0) < 0) {
        fprintf(stderr, "Failed to retrieve input stream information");
        av_packet_free(&pkt);
        avformat_close_input(&ifmt_ctx);
        goto err;
    }
    //////////////////////////////////////////////////////////////////////

    stream = ifmt_ctx->streams[0];
    dec = avcodec_find_decoder(stream->codecpar->codec_id);
    if (!dec) {
        av_log(NULL, AV_LOG_ERROR, "Failed to find decoder for stream\n");
        av_packet_free(&pkt);
        avformat_close_input(&ifmt_ctx);
        goto err;
    }
    pCodecCtx = avcodec_alloc_context3(dec);
    if (!pCodecCtx) {
        av_log(NULL, AV_LOG_ERROR, "Failed to allocate the decoder context for stream\n");
        av_packet_free(&pkt);
        avformat_close_input(&ifmt_ctx);
        goto err;
    }
    if (avcodec_open2(pCodecCtx, dec, NULL) < 0){
        fprintf(stderr, "avcodec_open2 error\n");
        av_packet_free(&pkt);
        avformat_close_input(&ifmt_ctx);
        goto err;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    av_dump_format(ifmt_ctx, 0, url, 0);    //вывод информации о входном потоке

    while (1) {
        ret = av_read_frame(ifmt_ctx, pkt);
        if (ret < 0)
            break;
        ///////////////////////////////////////////////////////////
        if (pkt->stream_index == 0){
            if ((pkt->flags & AV_PKT_FLAG_KEY) && count_replay)
                flag_prewiew = true;
            if ((pkt->flags & AV_PKT_FLAG_KEY) || flag_prewiew) {
                int ret;
                ret = avcodec_send_packet(pCodecCtx, pkt);
                if (ret == 0) {
                    AVFrame* frame = av_frame_alloc();
                    while (ret >= 0) {
                        ret = avcodec_receive_frame(pCodecCtx, frame);
                        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
                            break;
                        }
                        else if (ret < 0) {
                            qDebug()<<stderr<<"Error during decoding\n";
                            break;
                        }
                        else{
                            QImage img = avFrame2QImage(frame);
                            imgView(img);
                            if(count_replay){
                                imgReplay(img, cam);
                                count_replay--;
                            }else
                                flag_prewiew = false;
                        }
                    }
                    av_frame_unref(frame);
                    av_frame_free(&frame);
                }
            }//if key frame
        }
        //end_preview:
        if(!turn || mode_camera)
            break;

    }   //end while
    av_packet_free(&pkt);
    avformat_close_input(&ifmt_ctx);
    if (ret < 0 && ret != AVERROR_EOF) {
        goto err;
    }
err:
    qDebug() << "err";
    //emit err();
}

void FFmpegThread::processRecord(){
    const AVOutputFormat* ofmt;
    AVFormatContext *ifmt_ctx = NULL;
    AVFormatContext* ofmt_ctx = NULL;
    AVPacket *pkt = NULL;
    const char *url;
    int ret; //i;
    QByteArray baout;

    int stream_mapping_size = 0;
    int* stream_mapping = NULL;
    int stream_index = 0;

    const char* out_file;
    QString out_filename;

    AVCodecContext *pCodecCtx = NULL;
    AVStream *stream;
    const AVCodec *dec;
    bool flag_prewiew = false;
    QByteArray bain = Url.toLocal8Bit();
    url = bain.data();
    pkt = av_packet_alloc();
    if (!pkt) {
        qDebug()<<"Could not allocate AVPacket\n";
        goto err;
    }
    while(avformat_open_input(&ifmt_ctx, url, 0, 0) < 0){
        if(turn == 0){
            av_packet_free(&pkt);
            goto err;
        }
    }
    if (avformat_find_stream_info(ifmt_ctx, 0) < 0) {
        qDebug()<<"Failed to retrieve input stream information";
        av_packet_free(&pkt);
        avformat_close_input(&ifmt_ctx);
        goto err;
    }
    //////////////////////////////////////////////////////////////////////

    stream = ifmt_ctx->streams[0];
    dec = avcodec_find_decoder(stream->codecpar->codec_id);
    if (!dec) {
        av_log(NULL, AV_LOG_ERROR, "Failed to find decoder for stream\n");
        av_packet_free(&pkt);
        avformat_close_input(&ifmt_ctx);
        goto err;
    }
    pCodecCtx = avcodec_alloc_context3(dec);
    if (!pCodecCtx) {
        av_log(NULL, AV_LOG_ERROR, "Failed to allocate the decoder context for stream\n");
        av_packet_free(&pkt);
        avformat_close_input(&ifmt_ctx);
        goto err;
    }
    if (avcodec_open2(pCodecCtx, dec, NULL) < 0){
        fprintf(stderr, "avcodec_open2 error\n");
        av_packet_free(&pkt);
        avformat_close_input(&ifmt_ctx);
        goto err;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    av_dump_format(ifmt_ctx, 0, url, 0);    //вывод информации о входном потоке
    //state = 1;

    out_filename = QString("video/cam") + QString::number(cam) + QString("_") + QTime::currentTime().toString("HH_mm_ss") +
                    QString(".mp4");
    playFile = out_filename;
    baout = (&out_filename)->toLocal8Bit();
    out_file = baout.data();
    qDebug()<<"start"<<cam<<QTime::currentTime().toString("HH_mm_ss");
    qDebug()<<out_file;

    if(avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_file) >= 0)
        qDebug()<<"avformat_alloc_output_context2 success\n";
    else
        qDebug()<<"avformat_alloc_output_context2 error\n";

    if (!ofmt_ctx) {
        fprintf(stderr, "Could not create output context\n");
        ret = AVERROR_UNKNOWN;

        return;//goto end;
    }

    stream_mapping_size = ifmt_ctx->nb_streams;
    stream_mapping = (int*)av_calloc(stream_mapping_size, sizeof(*stream_mapping));
    if (!stream_mapping) {
        ret = AVERROR(ENOMEM);
        qDebug()<<"!stream_mapping";
        return;//goto end;
    }

    ofmt = ofmt_ctx->oformat;

    for (int i = 0; i < ifmt_ctx->nb_streams; i++) {
        AVStream *out_stream;
        AVStream *in_stream = ifmt_ctx->streams[i];
        AVCodecParameters *in_codecpar = in_stream->codecpar;

        if (in_codecpar->codec_type != AVMEDIA_TYPE_AUDIO &&
            in_codecpar->codec_type != AVMEDIA_TYPE_VIDEO &&
            in_codecpar->codec_type != AVMEDIA_TYPE_SUBTITLE) {
            stream_mapping[i] = -1;
            continue;
        }

        stream_mapping[i] = stream_index++;

        out_stream = avformat_new_stream(ofmt_ctx, NULL);
        if (!out_stream) {
            fprintf(stderr, "Failed allocating output stream\n");
            ret = AVERROR_UNKNOWN;
            qDebug()<<"Failed allocating output stream\n";
            return; //goto end;
        }

        ret = avcodec_parameters_copy(out_stream->codecpar, in_codecpar);
        if (ret < 0) {
            fprintf(stderr, "Failed to copy codec parameters\n");
            qDebug()<<"Failed to copy codec parameters\n";
            return; //goto end;
        }
        out_stream->codecpar->codec_tag = 0;
    }

    av_dump_format(ofmt_ctx, 0, out_file, 1);

    if (!(ofmt->flags & AVFMT_NOFILE)) {
        //ret = avio_open(&ofmt_ctx->pb, out_file, AVIO_FLAG_WRITE);
        ret = avio_open(&ofmt_ctx->pb, out_file, AVIO_FLAG_WRITE);
        if (ret < 0) {
            fprintf(stderr, "Could not open output file '%s'", out_file);
            qDebug()<<"Could not open output file";
            return; //goto end;
        }
    }

    ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0) {
        fprintf(stderr, "Error occurred when opening output file\n");
        qDebug()<<"Error occurred when opening output file";
        return; //goto end;
    }
    qDebug()<<out_filename;


    while (1) {
        AVStream *in_stream, *out_stream;
        ret = av_read_frame(ifmt_ctx, pkt);
        if (ret < 0)
            break;
        ///////////////////////////////////////////////////////////
        if (pkt->stream_index == 0){
            if ((pkt->flags & AV_PKT_FLAG_KEY) && count_replay)
                flag_prewiew = true;
            if ((pkt->flags & AV_PKT_FLAG_KEY) || flag_prewiew) {
                int ret;
                ret = avcodec_send_packet(pCodecCtx, pkt);
                if (ret == 0) {
                    AVFrame* frame = av_frame_alloc();
                    while (ret >= 0) {
                        ret = avcodec_receive_frame(pCodecCtx, frame);
                        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
                            break;
                        }
                        else if (ret < 0) {
                            qDebug()<<stderr<<"Error during decoding\n";
                            break;
                        }
                        else{
                            QImage img = avFrame2QImage(frame);
                            imgView(img);
                            if(count_replay){
                                imgReplay(img, cam);
                                count_replay--;
                            }else
                                flag_prewiew = false;
                        }
                    }
                    av_frame_unref(frame);
                    av_frame_free(&frame);
                }
            }//if key frame
        }   //end_preview:
        in_stream  = ifmt_ctx->streams[pkt->stream_index];
        if (pkt->stream_index >= stream_mapping_size ||
            stream_mapping[pkt->stream_index] < 0) {
            av_packet_unref(pkt);
            continue;
        }
        pkt->stream_index = stream_mapping[pkt->stream_index];
        out_stream = ofmt_ctx->streams[pkt->stream_index];
        log_packet(ifmt_ctx, pkt, "in");
        // copy packet
        av_packet_rescale_ts(pkt, in_stream->time_base, out_stream->time_base);
        //pkt->pts = av_rescale_q(pkt->pts, in_stream->time_base, out_stream->time_base);
        //pkt->dts = av_rescale_q(pkt->dts, in_stream->time_base, out_stream->time_base);
        //pkt->duration = av_rescale_q(pkt->duration, in_stream->time_base, out_stream->time_base);
        pkt->pos = -1;
        log_packet(ofmt_ctx, pkt, "out");
        ret = av_interleaved_write_frame(ofmt_ctx, pkt);
        // pkt is now blank (av_interleaved_write_frame() takes ownership of
        // its contents and resets pkt), so that no unreferencing is necessary.
        // This would be different if one used av_write_frame().
        if (ret < 0) {
            fprintf(stderr, "Error muxing packet\n");
            break;
        }
        if(!turn || !mode_camera)
            break;

    }   //end while
    av_write_trailer(ofmt_ctx);
    av_packet_free(&pkt);
    avformat_close_input(&ifmt_ctx);

    //close output
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
        avio_closep(&ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);
    av_freep(&stream_mapping);

    if (ret < 0 && ret != AVERROR_EOF) {
        goto err;
    }
err:
    qDebug() << "err";
    //emit err();
}




#include "camera.h"
#include <QDebug>
#include <QThread>

Camera::Camera(QObject *parent) : QObject(parent){
    url = QString("");
    file = "";
    flag_record = 0;
}

void Camera::StartRecord(QString s){
    if(!flag_record){
        file = s;
        flag_record = 1;
    }
}

void Camera::StopRecord(){
    flag_record = 4;
}

void Camera::setUrl(QString u){
    url = u;
}

void Camera::TurnOnCamera(){
    flag_record = 0;
    process = true;
    ofmt_ctx = NULL;
    ofmt = NULL;
    AVFormatContext *ifmt_ctx = NULL;

    AVPacket *pkt = NULL;
    const char *in_filename;

    int best_video;

    int ret;

    int *stream_mapping = NULL;
    int stream_mapping_size = 0;
    AVCodecContext *pCodecCtx = NULL;
    AVStream *stream;
    const AVCodec *dec;

    QByteArray bain = url.toLocal8Bit();
    in_filename = bain.data();


    pkt = av_packet_alloc();
    if (!pkt) {
        goto end;
    }

    if ((ret = avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) < 0) {;
        goto end;
    }

    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
        goto end;
    }

    best_video = av_find_best_stream(ifmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &dec, 0);
    if (best_video < 0) {
        goto end;
    }
    stream = ifmt_ctx->streams[best_video];
    dec = avcodec_find_decoder(stream->codecpar->codec_id);

    if (!dec) {
        goto end;
    }
    pCodecCtx = avcodec_alloc_context3(dec);
    if (!pCodecCtx) {
        goto end;
    }

    avcodec_parameters_to_context(pCodecCtx, ifmt_ctx->streams[best_video]->codecpar);

    if (avcodec_open2(pCodecCtx, dec, NULL) < 0){
        goto end;
    }

    stream_mapping_size = ifmt_ctx->nb_streams;
    stream_mapping = (int*)av_calloc(stream_mapping_size, sizeof(*stream_mapping));
    if (!stream_mapping) {
        goto end;
    }

    /////////////////////////////////////////////////////////

    while (process) {
        AVStream *in_stream, *out_stream;
        ret = av_read_frame(ifmt_ctx, pkt);
        if (ret < 0)
            break;
        if (pkt->stream_index == best_video){
            if (pkt->flags & AV_PKT_FLAG_KEY) {
                if(flag_record == 1){
                    ret = prepareRecord(ifmt_ctx, stream_mapping, file);
                    if(ret == -1) flag_record = 0;
                    else flag_record = 2;
                }
            }
                int ret;
                ret = avcodec_send_packet(pCodecCtx, pkt);
                if (ret < 0)
                    goto end_preview;
                AVFrame* frame = av_frame_alloc();
                while (ret >= 0) {
                    ret = avcodec_receive_frame(pCodecCtx, frame);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
                        av_frame_free(&frame);
                        goto end_preview;
                    }
                    else if (ret < 0) {
                        qDebug()<<"ret avcodec_receive_frame";
                        av_frame_free(&frame);
                        goto end_preview;
                    }
                    else{
                        emit sigImage(avFrame2QImage(frame));
                    }
                }
            //}//if key frame
        }

 end_preview:
        in_stream  = ifmt_ctx->streams[pkt->stream_index];
        if (pkt->stream_index >= stream_mapping_size ||
            stream_mapping[pkt->stream_index] < 0) {
            av_packet_unref(pkt);
            continue;
        }
        if(flag_record == 2){
            flag_record = 3;
            _dts = pkt->dts;
            _pts = pkt->pts;
        }
        if(flag_record == 3){
            pkt->stream_index = stream_mapping[pkt->stream_index];
            out_stream = ofmt_ctx->streams[pkt->stream_index];

            pkt->dts = pkt->dts - _dts;
            pkt->pts = pkt->pts - _pts;

            av_packet_rescale_ts(pkt, in_stream->time_base, out_stream->time_base);
            pkt->pos = -1;

            ret = av_interleaved_write_frame(ofmt_ctx, pkt);

            if (ret < 0)
                break;
        }

        if(flag_record == 4){
            av_write_trailer(ofmt_ctx);
            if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
                avio_closep(&ofmt_ctx->pb);
            avformat_free_context(ofmt_ctx);
            flag_record = 0;
        }

    }
end:
    if(flag_record == 3){
        av_write_trailer(ofmt_ctx);
        if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE)){
            avio_closep(&ofmt_ctx->pb);
            qDebug()<<"avio_closep";
        }
        avformat_free_context(ofmt_ctx);
        flag_record = 0;
    }

    av_packet_free(&pkt);
    avformat_close_input(&ifmt_ctx);
    avcodec_free_context(&pCodecCtx);
    av_freep(&stream_mapping);
    qDebug()<<"av_freep";

    emit finished();
}

void Camera::TurnOffCamera(){
    process = false;
}

QImage Camera::avFrame2QImage(AVFrame* frame){
    struct SwsContext *img_convert_ctx = NULL;
    img_convert_ctx = sws_getContext(frame->width, frame->height,
                                    (AVPixelFormat)frame->format, frame->width, frame->height,
                                     AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);

    if (img_convert_ctx == nullptr)
    {
            return QImage();
    }

    QImage img(frame->width, frame->height, QImage::Format_RGB888);
    int outputLinesize[3];
    av_image_fill_linesizes(outputLinesize, AV_PIX_FMT_RGB24, frame->width);
    uint8_t *outputDst[] = {img.bits()};
    sws_scale(img_convert_ctx, frame->data, frame->linesize, 0, frame->height, outputDst, outputLinesize);

    sws_freeContext(img_convert_ctx);
    return img;
}

int Camera::prepareRecord(AVFormatContext * ifmt, int *stream_mapp, QString file){
    QByteArray baout = file.toLocal8Bit();
    const char* out_file = baout.data();
    int ret;
    int stream_index = 0;
    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_file);
    if (!ofmt_ctx)
        goto err;

    ofmt = ofmt_ctx->oformat;

    for (uint i = 0; i < ifmt->nb_streams; i++) {
        AVStream *out_stream;
        AVStream *in_stream = ifmt->streams[i];
        AVCodecParameters *in_codecpar = in_stream->codecpar;

        if (in_codecpar->codec_type != AVMEDIA_TYPE_AUDIO &&
            in_codecpar->codec_type != AVMEDIA_TYPE_VIDEO &&
            in_codecpar->codec_type != AVMEDIA_TYPE_SUBTITLE) {
            stream_mapp[i] = -1;
            continue;
        }

        stream_mapp[i] = stream_index++;

        out_stream = avformat_new_stream(ofmt_ctx, NULL);
        if (!out_stream)
            goto err;

        ret = avcodec_parameters_copy(out_stream->codecpar, in_codecpar);
        if (ret < 0)
            goto err;
        out_stream->codecpar->codec_tag = 0;
    }
    av_dump_format(ofmt_ctx, 0, out_file, 1);

    if (!(ofmt_ctx->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, out_file, AVIO_FLAG_WRITE);
        if (ret < 0)
            goto err;
    }

    ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0)
        goto err;
    return 0;
err: return -1;
}

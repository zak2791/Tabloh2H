#include "camera.h"
#include <QDebug>
#include <QThread>

Camera::Camera(QObject *parent) : QObject(parent){
    url = QString("");
    file = "";
    flag_record = 0;
    only_key_frame = true;
}

void Camera::StartRecord(QString s){
    if(!flag_record){
        file = s;
        flag_record = 1;
    }
}

void Camera::StopRecord(){
    if(flag_record)
        flag_record = 4;
}

void Camera::setUrl(QString u){
    url = u;
}

void Camera::TurnOnCamera(){
    flag_record = 0;
    process = true;
    ofmt_ctx = NULL;

    //avformat_network_init();

    ofmt = NULL;
    ifmt_ctx = NULL;

    AVPacket* pkt = NULL;
    //AVPacket* pkt_stream = NULL;

    const char *in_filename;

    int best_video;

    int ret;

    stream_mapping = NULL;
    int stream_mapping_size = 0;
    AVCodecContext *pCodecCtx = NULL;
    AVStream *stream;
    const AVCodec *dec;

    QByteArray bain = url.toLocal8Bit();
    in_filename = bain.data();

    //AVFormatContext* octx = NULL;////
    //AVDictionary *options = NULL; ////

    pkt = av_packet_alloc();
    if (!pkt) {
        goto end;
    }


    //ret = avformat_alloc_output_context2(&ofmt_ctx_stream, NULL, "flv", "rtmp://192.168.1.100");/////
    //qDebug()<<"err = "<<ret;

    //av_dict_set(&options, "live", "1", 0);/////
    //ret = avformat_write_header(octx, &options);//////
    //qDebug()<<"avformat_write_header = "<<ret;

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
    //avcodec_parameters_to_context(octx, ifmt_ctx->streams[best_video]->codecpar);

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
                //int ret;
            if(pkt->flags & AV_PKT_FLAG_KEY || !only_key_frame){
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

            }//if key frame
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
        if(flag_stream){
            AVPacket pkt_ref = { 0 };
            //qDebug()<<"flag_stream = "<<flag_stream;
            ret = av_packet_ref(&pkt_ref, pkt);
            //qDebug()<<"ret = "<<ret;
            if(ret < 0)
                qDebug()<<"error av_packet_ref = "<<ret;
            else{
            pkt_ref.stream_index = stream_mapping[pkt_ref.stream_index];
            out_stream = ofmt_ctx_stream->streams[pkt_ref.stream_index];

            //pkt->dts = pkt->dts - _dts;
            //pkt->pts = pkt->pts - _pts;

            av_packet_rescale_ts(&pkt_ref, in_stream->time_base, out_stream->time_base);
            pkt_ref.pos = -1;

            ret = av_interleaved_write_frame(ofmt_ctx_stream, &pkt_ref);

            if (ret < 0)
                qDebug()<<"av_interleaved_write_frame stream= "<<ret;
            }
        }

        if(flag_record == 3){
            pkt->stream_index = stream_mapping[pkt->stream_index];
            out_stream = ofmt_ctx->streams[pkt->stream_index];

            pkt->dts = pkt->dts - _dts;
            pkt->pts = pkt->pts - _pts;

            av_packet_rescale_ts(pkt, in_stream->time_base, out_stream->time_base);
            pkt->pos = -1;

            ret = av_interleaved_write_frame(ofmt_ctx, pkt);
            //ret = av_write_frame(octx, pkt); //////
            //qDebug()<<"av_write_frame = "<<ret;

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

void Camera::onlyKeyFrame(bool b){
    only_key_frame = b;
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
    //avformat_alloc_output_context2(&ofmt_ctx, NULL, "flv", "rtmp://192.168.1.100");
    qDebug()<<out_file;
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

void Camera::startStream(QString url){
    if(ifmt_ctx == NULL)
        return;
    qDebug()<<"startStream -1";
    int ret;
    int stream_index = 0;
    const char* out_filename = url.toUtf8().constData();    //"rtmp://192.168.1.100/live/stream";
    qDebug()<<"startStream 0";
    avformat_alloc_output_context2(&ofmt_ctx_stream, NULL, "flv", out_filename);
    if (!ofmt_ctx_stream){
        qDebug()<<"!ofmt_ctx_stream";
        goto err;
    }
    qDebug()<<"startStream 1";
    ofmt_stream = ofmt_ctx_stream->oformat;

    for (uint i = 0; i < ifmt_ctx->nb_streams; i++) {
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

        out_stream = avformat_new_stream(ofmt_ctx_stream, NULL);
        if (!out_stream){
            qDebug()<<"out_stream"<<out_stream;
            goto err;
        }

        ret = avcodec_parameters_copy(out_stream->codecpar, in_codecpar);
        if (ret < 0){
            qDebug()<<"avcodec_parameters_copy"<<ret;
            goto err;
        }
        out_stream->codecpar->codec_tag = 0;
    }
    qDebug()<<"startStream 2";
    av_dump_format(ofmt_ctx_stream, 0, out_filename, 1);

    if (!(ofmt_stream->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx_stream->pb, out_filename, AVIO_FLAG_WRITE);
        if (ret < 0){
            qDebug()<<"avio_open"<<ret;
            goto err;
        }
    }
    qDebug()<<"startStream 3";
    ret = avformat_write_header(ofmt_ctx_stream, NULL);
    if (ret < 0){
        qDebug()<<"avformat_write_header"<<ret;
        goto err;
    }
    qDebug()<<"startStream 4";
    //return 0;
    flag_stream = true;
    qDebug()<<"ok turn stream";
    return;
err:
    qDebug()<<"err stream turn";   //return -1;
    emit errStream();
}

void Camera::stopStream(){
    //av_write_trailer(ofmt_ctx_stream);
    qDebug()<<"stop stream0";
    // if(ofmt_ctx_stream == NULL)
    //     return;
    if (ofmt_ctx_stream && !(ofmt_stream->flags & AVFMT_NOFILE)){
        avio_closep(&ofmt_ctx_stream->pb);
        qDebug()<<"avio_closep ofmt_ctx_stream";
    }
    avformat_free_context(ofmt_ctx_stream);
    flag_stream = false;
    qDebug()<<"stop stream";
}

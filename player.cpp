#include "player.h"
#include <QDebug>
#include <QThread>

Player::Player(QString file, QObject *parent) : QObject(parent){
    videoFile = file;
}

void Player::Play(){
    process = true;
    int best_stream;
    int numberFrames;
    int durationMedia;
    int avgFps;
    int durationMediaInSecunds;
    bufImage = new  QList<QImage>;

    AVFormatContext *ifmt_ctx = NULL;

    AVPacket *pkt = NULL;
    const char *in_filename;

    int ret;

    int *stream_mapping = NULL;
    int stream_mapping_size = 0;
    AVCodecContext *pCodecCtx = NULL;
    AVStream *stream;
    const AVCodec *dec;

    QByteArray bain = videoFile.toLocal8Bit();
    in_filename = bain.data();

    pkt = av_packet_alloc();
    if (!pkt)
        goto end;

    if ((ret = avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) < 0)
        goto end;

    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0)
        goto end;

    ret = av_find_best_stream(ifmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &dec, 0);
    if (ret < 0)
        goto end;

    best_stream = ret;

    stream = ifmt_ctx->streams[ret];

    dec = avcodec_find_decoder(stream->codecpar->codec_id);
    if (!dec)
        goto end;

    pCodecCtx = avcodec_alloc_context3(dec);
    if (!pCodecCtx)
        goto end;

    avcodec_parameters_to_context(pCodecCtx, ifmt_ctx->streams[best_stream]->codecpar);


    if (avcodec_open2(pCodecCtx, dec, NULL) < 0)
        goto end;

    stream_mapping_size = ifmt_ctx->nb_streams;
    stream_mapping = (int*)av_calloc(stream_mapping_size, sizeof(*stream_mapping));
    if (!stream_mapping) {
        goto end;
    }
    /////////////////////////////////////////////////////////

    AVStream *in_stream;
    in_stream = ifmt_ctx->streams[best_stream];
    numberFrames = in_stream->nb_frames;
    durationMedia = in_stream->duration;
    avgFps = in_stream->avg_frame_rate.num / in_stream->avg_frame_rate.den;
    oneFrameDuration = durationMedia / numberFrames;
    durationMediaInSecunds = numberFrames / avgFps;

    emit sigParam(numberFrames, avgFps, durationMediaInSecunds);

    while (process) {
        if(flag_seek || flag_play || flag_one_next_frame){
            if(flag_seek){
                ret =av_seek_frame(ifmt_ctx, best_stream, flag_seek, AVSEEK_FLAG_FRAME);
                qDebug()<<"flag_seek = "<<flag_seek;
                flag_seek = 0;
                //qDebug()<<"seek = 0";
                bufImage->clear();
            }
            ret = av_read_frame(ifmt_ctx, pkt);
            if (ret < 0){
                ret =av_seek_frame(ifmt_ctx, best_stream, 0, AVSEEK_FLAG_FRAME);
                if (ret < 0 ) break;
                continue;
            }

            if (pkt->stream_index == best_stream){
                if(flag_play || flag_one_next_frame)
                    emit sigFrame(pkt->pts / oneFrameDuration);
                int cel = (pkt->pts / oneFrameDuration) * 0.033;
                float drob = (pkt->pts / oneFrameDuration) * 0.033 - cel;
                QString min = "";
                QString sec = "";
                QString msec = "";
                if(cel / 60){
                    min = QString::number(cel / 60) + ":";
                    int s = cel % 60;
                    if(s < 10)
                        sec = "0" + QString::number(s);
                    else
                        sec = QString::number(s);
                }else{
                    min = "0:";
                    if(cel < 10)
                        sec = "0" + QString::number(cel);
                    else
                        sec = QString::number(cel);
                }
                msec = QString::number(drob).remove(0, 1);

                emit sigTime(min + sec + msec);
                ret = avcodec_send_packet(pCodecCtx, pkt);
                if (ret < 0) {
                    goto end_preview;
                }
                AVFrame* frame = av_frame_alloc();
                while (ret >= 0) {
                    ret = avcodec_receive_frame(pCodecCtx, frame);
                    if (ret == AVERROR(EAGAIN)){
                        av_frame_free(&frame);
                        goto end_preview;
                    }
                    else if (ret == AVERROR_EOF){
                        av_frame_free(&frame);
                        goto end_preview;
                    }
                    else if (ret < 0) {
                        av_frame_free(&frame);
                        goto end_preview;
                    }
                    else{
                        if(!flag_one_next_frame)
                            QThread::msleep(50);
                        QImage img = avFrame2QImage(frame);
                        if(flag_play || flag_one_next_frame){
                            flag_one_next_frame = false;
                            bufImage->append(img);
                            if(bufImage->length() > avgFps * 2)
                                bufImage->removeFirst();
                            currentImage = bufImage->length() - 1;
                        }
                        emit sigBuffer(bufImage->length(), bufImage->length() - 1);
                        emit sigImage(img);
                    }
                }
            }
            end_preview:
            av_packet_unref(pkt);
        }   //if seek
    }   //while process
end:
    av_packet_free(&pkt);
    avformat_close_input(&ifmt_ctx);
    avcodec_free_context(&pCodecCtx);
    av_freep(&stream_mapping);
}

void Player::seek(int s){
    flag_seek = s * oneFrameDuration;
    currentImage = -1;
    qDebug()<<"flag_seek = "<<s<<oneFrameDuration<<flag_seek;
}

void Player::turnPlay(){
    if(flag_play){
        flag_play = false;
        currentImage = bufImage->length() - 1;
        emit sigStartStopPlay(true);
    }
    else{
        flag_play = true;
        emit sigStartStopPlay(false);
    }
}

QImage Player::avFrame2QImage(AVFrame* frame){
    struct SwsContext *img_convert_ctx = NULL;
    img_convert_ctx = sws_getContext(frame->width, frame->height,
                                    (AVPixelFormat)frame->format, frame->width, frame->height,
                                     AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);

    if (img_convert_ctx == nullptr){
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

void Player::turnOff(){
    process = false;
    flag_play = false;
    bufImage->clear();
}

void Player::nextFrame(){
    if(!flag_play){
        if(currentImage < bufImage->length() - 1){
            emit sigImage(bufImage->at(++currentImage));
            emit sigBuffer(bufImage->length(), currentImage);
        }
        else{
            flag_one_next_frame = true;
        }
    }
}

void Player::previewFrame(){
    //qDebug()<<"currentImage"<<currentImage<<"bufImage->length()"<<bufImage->length();
    if(currentImage > 0 && !flag_play){
        emit sigImage(bufImage->at(--currentImage));
        emit sigBuffer(bufImage->length(), currentImage);
    }
}

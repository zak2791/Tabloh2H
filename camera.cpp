#include "camera.h"
#include <QDebug>
#include <QThread>
#include <QCamera>
#include <QCameraInfo>

#include <QProcess>

static const std::string av_make_error_string(int errnum)
{
    char errbuf[AV_ERROR_MAX_STRING_SIZE];
    av_strerror(errnum, errbuf, AV_ERROR_MAX_STRING_SIZE);
    return (std::string)errbuf;
}

Camera::Camera(QObject *parent) : QObject(parent){
    url = QString("");
    file = "";
    flag_record = 0;
    flag_start_record = false;
    only_key_frame = true;
    avdevice_register_all();
    //inFormat = NULL;

}

void Camera::StartRecord(QString s){
    if(!flag_record){
        file = s;
        //flag_start_record = true;
        //process = false;
        qDebug()<<"start0";
        //int i = 0;
        //while(flag_start_record){
            //qDebug()<<i++;
        //};
        qDebug()<<"start1";
        flag_record = true;
        //process = true;
    }
}

void Camera::StopRecord(){
    if(flag_record)
        flag_record = 4;
}

void Camera::setUrl(QString u){
    url = u;
}

QStringList Camera::getListWebCams()
{
    QStringList listWebCam;

    const QList<QCameraInfo> cams = QCameraInfo::availableCameras();
    for (const QCameraInfo &cameraInfo : cams){
        QStringList list = getListParamWebCam(cameraInfo.description());
        if(list.count() != 0)
            listWebCam<<cameraInfo.description();
    }

    return listWebCam;
}

QStringList Camera::getListSoundDevices()
{
    QStringList listWebCam;
    AVDeviceInfoList* deviceList;
    const AVInputFormat* inFormat = NULL;
    inFormat = av_find_input_format("dshow");
    int deviceCount = avdevice_list_input_sources(inFormat, NULL, NULL, &deviceList);
    for(int i = 0; i < deviceCount; i++){
        AVDeviceInfo dInfo = *deviceList->devices[i];
        listWebCam.append(dInfo.device_description);
    }
    avdevice_free_list_devices(&deviceList);

    return listWebCam;
}

QStringList Camera::getListParamWebCam(QString text)
{
    QStringList param;
    QCamera* cam;
    const QList<QCameraInfo> cams = QCameraInfo::availableCameras();
    for (const QCameraInfo &cameraInfo : cams) {
        qDebug()<<cameraInfo.deviceName()<<cameraInfo.description();
        if (cameraInfo.description() == text){
            cam = new QCamera(cameraInfo);
            break;
        }
    }

    if(cam->isAvailable())
        cam->start();

    int i = 0;

    QList<QCameraViewfinderSettings> ViewSets = cam->supportedViewfinderSettings();
    qDebug() << "viewfinderResolutions sizes.len = " << ViewSets.length();
    foreach (QCameraViewfinderSettings ViewSet, ViewSets) {
        param<<"fps = " + QString::number(ViewSet.maximumFrameRate()) + "  resolution = " +
                     QString::number(ViewSet.resolution().rwidth()) + "x" +
                     QString::number(ViewSet.resolution().rheight());
        qDebug()<<ViewSet.pixelFormat();
    }
    cam->stop();

    return param;
}

void Camera::TurnOnCamera(){

    //flag_record = rec;
    process = true;
    ofmt_ctx = NULL;

    ofmt = NULL;

    int *stream_mapping = NULL;
    int stream_mapping_size = 0;
    AVCodecContext *pCodecCtx = NULL;
    AVStream *stream;
    const AVCodec *dec;
    AVDictionary *options = NULL;

    QByteArray bain = url.toLocal8Bit();
    const char *in_filename;
    in_filename = bain.data();
    AVFormatContext *ifmt_ctx = NULL;
    qDebug()<<"turn"<<url;


    AVInputFormat* inFormat = NULL;
    int best_video;
    int ret;
    AVPacket *pkt = NULL;

    if(url.left(4) == "rtsp")
        av_dict_set(&options, "rtsp_transport", "tcp", 0);
    else{
        //if(inFormat == NULL){
        QStringList listUrl = url.split(";");
        if(listUrl.count() == 2){               //если выбрана web камера
            url = "video=" + listUrl.at(0);//  + ":audio=@device_pnp_\\\\?\\usb#vid_04f2&pid_b729&mi_00#6&191d7230&0&0000#{65e8773d-8f56-11d0-a3b9-00a0c9223196}\global";
            bain = url.toLocal8Bit();
            in_filename = bain.data();
            QString fps = listUrl.at(1).split(" ").at(2);
            QString resolution = listUrl.at(1).split(" ").at(6);
            qDebug()<<url<<fps<<resolution;
            const char *pixel_format = "yuvj422p";
            av_dict_set(&options, "video_size", resolution.toStdString().c_str(), 0);
            //av_dict_set(&options, "framerate", fps.toStdString().c_str(), 0);
            av_dict_set(&options, "framerate", "30", 0);
            av_dict_set(&options, "r", "30", 0);
            av_dict_set(&options, "buffer_size", "1024000", 0);
            av_dict_set(&options, "b:v", "2.5M", 0);
            //av_dict_set(&options, "pixel_format", pixel_format, 0);
            //av_dict_set(&options, "pixel_format", "yuv422p", 0);
            //av_dict_set(&options, "vcodec", "h265", 0);
            inFormat = (AVInputFormat*)av_find_input_format("dshow");

        }



        //AVFormatContext* inFormatContext = avformat_alloc_context();;
        // if (!inFormat || !inFormat->priv_class  || !AV_IS_INPUT_DEVICE(inFormat->priv_class->category))
        // {
        //     return -1;
        // }
        //avdevice_free_list_devices(&deviceList);
        //avformat_free_context(inFormatContext);
        //printf("%i %i", inFormat, deviceCount);

        pkt = av_packet_alloc();
        if (!pkt) {
            goto end;
        }

        qDebug()<<"inFormat = "<<inFormat;

        //}
    }
    qDebug()<<"url = "<<url<<in_filename;
    //QThread::sleep(10);
    qDebug()<<"inFormat = "<<inFormat;
    if ((ret = avformat_open_input(&ifmt_ctx, in_filename, inFormat, &options)) < 0) {
        qDebug()<<"ret0 = "<<ret<<QString::fromStdString(av_make_error_string(ret));
        goto end;
    }
    qDebug()<<"0";
    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
        goto end;
    }
    qDebug()<<"1"<<ifmt_ctx->nb_streams;
    best_video = av_find_best_stream(ifmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &dec, 0);
    if (best_video < 0) {
        goto end;
    }
    qDebug()<<"2";
    stream = ifmt_ctx->streams[best_video];
    dec = avcodec_find_decoder(stream->codecpar->codec_id);
    qDebug()<<"3";
    if (!dec) {
        goto end;
    }
    pCodecCtx = avcodec_alloc_context3(dec);
    qDebug()<<"4";
    if (!pCodecCtx) {
        goto end;
    }
    qDebug()<<"5";
    avcodec_parameters_to_context(pCodecCtx, ifmt_ctx->streams[best_video]->codecpar);

    qDebug()<<"fmp = "<<pCodecCtx->pix_fmt;


    if (avcodec_open2(pCodecCtx, dec, NULL) < 0){
        goto end;
    }
    qDebug()<<"6";
    stream_mapping_size = ifmt_ctx->nb_streams;
    stream_mapping = (int*)av_calloc(stream_mapping_size, sizeof(*stream_mapping));
    if (!stream_mapping) {
        goto end;
    }
    qDebug()<<"7"<<"process = "<<process;
    /////////////////////////////////////////////////////////

    while (process) {
        AVStream *in_stream, *out_stream;

        //qDebug()<<"1";
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
            //qDebug()<<"pkt->flags = "<<pkt->flags;
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
        if(flag_record == 3){

            qDebug()<<"pkt->pts = "<<pkt->pts;
            pkt->stream_index = stream_mapping[pkt->stream_index];
            out_stream = ofmt_ctx->streams[pkt->stream_index];

            pkt->dts = pkt->dts - _dts + 30;
            pkt->pts = pkt->pts - _pts + 30;

            qDebug()<<"pts = "<<pkt->pts;
            qDebug()<<"dts = "<<pkt->dts;            av_packet_rescale_ts(pkt, in_stream->time_base, out_stream->time_base);

            pkt->pos = -1;

            ret = av_interleaved_write_frame(ofmt_ctx, pkt);

            if (ret < 0)
                //break;
                qDebug()<<"Error while writing output packet: %s\n";//<<av_err2str(ret);
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
        }
        avformat_free_context(ofmt_ctx);
        flag_record = 0;
    }

    av_packet_free(&pkt);
    avformat_close_input(&ifmt_ctx);
    avcodec_free_context(&pCodecCtx);
    av_freep(&stream_mapping);
    emit finished();
}

void Camera::onlyKeyFrame(bool b){
    only_key_frame = b;
}

void Camera::TurnOffCamera(){
    process = false;
    //main_process = false;
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

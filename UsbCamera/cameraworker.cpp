#include "cameraworker.h"

#include <QTcpServer>
#include <QTcpSocket>

#include <QFile>

char* _av_err2str(int errnum){
    char buf[AV_ERROR_MAX_STRING_SIZE];
    return av_make_error_string(buf, AV_ERROR_MAX_STRING_SIZE, errnum);
}

static const char* av_make_error(int errnum){
    static char str[AV_ERROR_MAX_STRING_SIZE];
    memset(str, 0, sizeof(str));
    return av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, errnum);
}

CameraWorker::CameraWorker(int cameraNumber, int w, int h, int fps, QString hwDec, bool sound, QString url, QObject* parent) :  QObject(parent) {
    if(hwDec == "нет")
        hwType = AV_HWDEVICE_TYPE_NONE;
    else if(hwDec == "cuda")
        hwType = AV_HWDEVICE_TYPE_CUDA;
    else if(hwDec == "dxva2")
        hwType = AV_HWDEVICE_TYPE_DXVA2;
    else if(hwDec == "d3d11va")
        hwType = AV_HWDEVICE_TYPE_D3D11VA;
    else if(hwDec == "d3d12va")
        hwType = AV_HWDEVICE_TYPE_D3D12VA;
    else if(hwDec == "vulkan")
        hwType = AV_HWDEVICE_TYPE_VULKAN;

    tmrCheckConnection = new QTimer(this);

    std::string s;
    if(cameraNumber == 1){
        videoPort = 5551;
        audioPort = 5552;
    }
    else if(cameraNumber == 2){
        videoPort = 5561;
        audioPort = 5562;
    }
    else{
        videoPort = 5571;
        audioPort = 5572;
    }
    isSound = sound;
    if(url == "")
        isStream = false;
    else{
        isStream = true;
        s = url.toStdString();
        urlVk = s.c_str();
        tmrCheckStream = new QTimer(this);
        connect(tmrCheckStream, &QTimer::timeout, this, &CameraWorker::checkStream);
        procCheckStream = new QProcess(this);
        procCheckStream->setProgram("ffprobe");
        procCheckStream->setArguments({"show_streams", url});
        procCheckStream->setReadChannel(QProcess::StandardError);

        connect(procCheckStream, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this](){
            if(procCheckStream->readAll().contains("error"))
                emit sigIsStream(false);
            else
                emit sigIsStream(true);
        });
        tmrCheckStream->start(10000);
    }
    configureError =configure(w, h, fps, isStream);
    if(configureError < 0){
        isStream = false;
    }
    // if(isStream){
    //     connect(tmrCheckStream, &QTimer::timeout, this, &CameraWorker::checkStream);
    //     tmrCheckStream->start(10000);
    // }

    videoSocket = new QTcpSocket(this);
    audioSocket = new QTcpSocket(this);

    connect(videoSocket, &QTcpSocket::readyRead, this, &CameraWorker::readVideoPacket);
    connect(audioSocket, &QTcpSocket::readyRead, this, &CameraWorker::readAudioPacket);

    connect(videoSocket, &QTcpSocket::connected, this, [this](){
        isConnect = true;

    });
    connect(videoSocket, &QTcpSocket::disconnected, this, [this](){
        isConnect = false;
    });

    connect(tmrCheckConnection, &QTimer::timeout, this, [this](){
        if(!isConnect)
            start();
    });

    tmrCheckConnection->start(1000);
}

CameraWorker::~CameraWorker()
{
    tmrCheckConnection->stop();
    videoSocket->close();
    if(isStream || isSound)
        audioSocket->close();
    // if(isStream)
    //     tmrCheckStream->stop();
}

void CameraWorker::start(){
    videoSocket->connectToHost(QHostAddress::LocalHost, videoPort);

    if(isStream || isSound)
        audioSocket->connectToHost(QHostAddress::LocalHost, audioPort);

    if(configureError == -1)
        emit sigExit();
}

AVCodecContext* CameraWorker::createDecoderContext(int width, int height, int fps)
{
    const AVCodec* codec;
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    AVCodecContext* context = avcodec_alloc_context3(codec);

    if (!context) {
        qDebug()<<"Could not allocate video codec context\n";
        return NULL;
    }

    context->coded_width = width;
    context->coded_height = height;
    context->pix_fmt = AVPixelFormat(-1);
    context->pkt_timebase = AVRational{1, 90000};
    context->codec_type = AVMEDIA_TYPE_VIDEO;
    context->sample_aspect_ratio = AVRational{0, 1};
    context->bit_rate = (width * height * fps) / 10;
    context->framerate = AVRational{fps, 1};

    context->codec_id = AV_CODEC_ID_H264;
    context->codec_tag = 0;

    if(hwType != AV_HWDEVICE_TYPE_NONE)
        qDebug()<<"hw = "<<av_hwdevice_ctx_create(&context->hw_device_ctx, hwType,
                                                      NULL, NULL, 0);
    if (avcodec_open2(context, codec, NULL) < 0) {
        qDebug()<<"Could not open codec\n";
        avcodec_free_context(&context);
        return NULL;
    }
    return context;
}

void CameraWorker::readVideoPacket()
{
    QTcpSocket* socket = static_cast<QTcpSocket*>(sender());
    while(socket->bytesAvailable()){
        int sizePacket;
        qlonglong timestamp;
        QByteArray baHeader = socket->read(7);
        int keyFrame;
        if(baHeader.contains("packet")){
            keyFrame = baHeader.contains("1") ? 1 : 0;
            QByteArray bTimestamp = socket->read(8);
            std::reverse(bTimestamp.begin(), bTimestamp.end());
            char* bDur = bTimestamp.data();
            memcpy(&timestamp, bDur, sizeof(qlonglong));
            //qDebug()<<"timestamp video = "<<timestamp;
            QByteArray baSize = socket->read(4);
            std::reverse(baSize.begin(), baSize.end());
            char* bytes = baSize.data();
            memcpy(&sizePacket, bytes, sizeof(int));
        }else{
            socket->readAll();
            return;
        }
        while(socket->bytesAvailable() < sizePacket)
            socket->waitForReadyRead();
        QByteArray ba = socket->read(sizePacket);
        packet p;
        p.data = ba;
        p.flags = keyFrame;
        p.pts = timestamp;
        videoHandler(p);
    }
}

void CameraWorker::checkStream()
{
    // qDebug()<<vkUrl<<urlVk;
    // procCheckStream = new QProcess(this);
    // procCheckStream->setProgram("ffprobe.exe");
    // procCheckStream->setArguments({vkUrl});
    // connect(procCheckStream, &QProcess::readyRead, this, [this](){
    //     qDebug()<<procCheckStream->readAllStandardOutput();
    // });
    qDebug()<<procCheckStream->arguments();
    procCheckStream->start();
}

void CameraWorker::readAudioPacket(){
    QTcpSocket* socketAudio = static_cast<QTcpSocket*>(sender());
    while(socketAudio->bytesAvailable()){
        int sizePacket;
        qlonglong timestamp;
        QByteArray baHeader = socketAudio->read(4);
        if(baHeader.contains("size")){
            QByteArray bTimestamp = socketAudio->read(8);
            std::reverse(bTimestamp.begin(), bTimestamp.end());
            char* bDur = bTimestamp.data();
            memcpy(&timestamp, bDur, sizeof(qlonglong));
            QByteArray baSize = socketAudio->read(4);
            std::reverse(baSize.begin(), baSize.end());
            char* bytes = baSize.data();
            memcpy(&sizePacket, bytes, sizeof(int));
        }else{
            socketAudio->readAll();
            return;
        }

        while(socketAudio->bytesAvailable() < sizePacket)
            socketAudio->waitForReadyRead();

        QByteArray ba = socketAudio->read(sizePacket);
        if(firstAac){
            firstAac = false;
            return;
        }
        packet p;
        p.pts = timestamp;
        p.data = ba;
        audioHandler(p);
    }
}

void CameraWorker::videoHandler(packet p)
{
    emit sigVideoPacket(p);
    int sizePacket = p.data.size();
    AVPacket *pPacket = av_packet_alloc();
    AVFrame *pFrame = av_frame_alloc();
    AVFrame* sw_frame = av_frame_alloc();

    uint8_t* data = new uint8_t[sizePacket];
    memcpy(data, p.data, sizePacket);
    pPacket->data = data;
    pPacket->size = sizePacket;
    pPacket->flags = p.flags;
    pPacket->pts = p.pts * 9 / 100;
    pPacket->dts = pPacket->pts;
    int keyFrame = p.flags;

    int ret = avcodec_send_packet(decoderContext, pPacket);
    if(ret < 0 || ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
        qDebug()<< "avcodec_send_packet: " << ret;
    }
    else{
        while(ret >= 0){

            ret = avcodec_receive_frame(decoderContext, pFrame);
            if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
                //qDebug() << "avcodec_receive_frame: " << ret;
            }
            else{
                if(decoderContext->pix_fmt == -1)
                    decoderContext->pix_fmt = (AVPixelFormat)pFrame->format;
                if(hwType != AV_HWDEVICE_TYPE_NONE){
                    ret = av_hwframe_transfer_data(sw_frame, pFrame, 0);
                    sw_frame->pts = pFrame->pts;
                }
                if (ret < 0) {
                    qDebug()<<"Error transferring the data to system memory";
                }
                if(keyFrame){
                    if(hwType != AV_HWDEVICE_TYPE_NONE)
                        emit sigFrame(avFrame2QImage(sw_frame));
                    else
                        emit sigFrame(avFrame2QImage(pFrame));
                }
                if(isStream){
                    if(!enabledFilter){
                        if(hwType != AV_HWDEVICE_TYPE_NONE)
                            qDebug()<<"initFilter = "<<initFilter(decoderContext, encoderContext, (AVPixelFormat)sw_frame->format);
                        else
                            qDebug()<<"initFilter = "<<initFilter(decoderContext, encoderContext, (AVPixelFormat)pFrame->format);
                    }
                    else{
                        if(hwType != AV_HWDEVICE_TYPE_NONE)
                            ret = filter_encode_write_frame(sw_frame);
                        else
                            ret = filter_encode_write_frame(pFrame);
                        if (ret < 0)
                            qDebug()<<"error filter_encode_write_frame ";
                        else{
                            AVPacket *pkt;
                            pkt = av_packet_alloc();
                            ret = avcodec_send_frame(encoderContext, filter_ctx->filtered_frame);
                            if (ret < 0) {
                                qDebug()<<"Error sending a frame for encoding\n";
                            }else{
                                while (ret >= 0) {
                                    ret = avcodec_receive_packet(encoderContext, pkt);
                                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                                        break;
                                    else if (ret < 0) {
                                        break;
                                    }

                                    int size = pkt->size - 4;

                                    QByteArray bSize = QByteArray::fromRawData(reinterpret_cast<const char *>(&size), sizeof(size));

                                    pkt->data[0] = bSize.at(3);
                                    pkt->data[1] = bSize.at(2);
                                    pkt->data[2] = bSize.at(1);
                                    pkt->data[3] = bSize.at(0);
                                    pkt->pts = p.pts / 1000;
                                    pkt->dts = pkt->pts;
                                    pkt->stream_index = 1;

                                    if(!firstAvvc){
                                        av_interleaved_write_frame(contextVk, pkt);
                                        avio_flush(contextVk->pb);
                                    }
                                    else
                                        firstAvvc = false;
                                    av_packet_unref(pkt);
                                }
                            }
                            av_frame_unref(filter_ctx->filtered_frame);
                        }
                    }
                }
                else{

                }
            }
        }
    }
    delete[] data;
    av_frame_free(&pFrame);
    av_frame_free(&sw_frame);
    av_packet_unref(pPacket);


}

void CameraWorker::audioHandler(packet p)
{
    emit sigSoundPacket(p);
    if(isStream){
        if(!firstAac){
            int sizePacket = p.data.size();

            AVPacket *pPacket = av_packet_alloc();

            uint8_t* data = new uint8_t[sizePacket];
            memcpy(data, p.data, sizePacket);
            pPacket->data = data;
            pPacket->size = sizePacket;
            pPacket->pts = p.pts / 1000;;
            pPacket->dts = pPacket->pts;
            pPacket->stream_index = 0;
            int ret = av_interleaved_write_frame(contextVk, pPacket);
            avio_flush(contextVk->pb);
            if(ret == 0){
                if(errStream){
                    errStream = false;
                    emit sigIsStream(true);
                }
            }
            else{
                if(!errStream){
                    errStream = true;
                    emit sigIsStream(false);
                }
            }

            delete[] data;
            av_packet_unref(pPacket);
        }
        else
            firstAac = false;
    }



}

int CameraWorker::configure(int w, int h, int f, bool vk){
    decoderContext = createDecoderContext(w, h, f);
    if(decoderContext == NULL) return -1;
    if(vk){
        isStream = true;
        const AVCodec *encoder;
        encoder = avcodec_find_encoder(AV_CODEC_ID_H264);
        encoderContext = avcodec_alloc_context3(encoder);
        if (!encoderContext) {
            qDebug()<<"Could not allocate video encoder context";
            return -2;
        }

        encoderContext->coded_width = w;
        encoderContext->coded_height = h;
        encoderContext->pix_fmt = AV_PIX_FMT_YUV420P;
        encoderContext->time_base = AVRational{1, 90000};
        encoderContext->pkt_timebase.den = 90000;
        encoderContext->pkt_timebase.num = 1;
        encoderContext->bit_rate = (w * h * f) / 10;
        encoderContext->gop_size = f;
        encoderContext->max_b_frames = 1;
        encoderContext->framerate = (AVRational){f, 1};

        encoderContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

        av_opt_set(encoderContext->priv_data, "preset", "ultrafast", 0);
        int ret;
        if ((ret = avcodec_open2(encoderContext, encoder, NULL)) < 0) {
            qDebug()<<"Could not open encoder "<<_av_err2str(ret);
            return -3;
        }

        avformat_alloc_output_context2(&contextVk, NULL, "flv", urlVk);
        if (!contextVk) {
            qDebug()<<"Could not deduce output format from file extension: using MPEG.";
            return -4;
        }

        if(addNewAudioStream(contextVk, 0) < 0)
            return -5;
        if(addNewVideoStream(contextVk, encoderContext, w, h, f, 1, true) < 0)
            return -6;

        if (!(contextVk->oformat->flags & AVFMT_NOFILE)) {
            ret = avio_open(&contextVk->pb, urlVk, AVIO_FLAG_WRITE);
            if (ret < 0) {
                qDebug()<<"Could not open output file '%s'"<<urlVk;
                return -7;
            }
        }
        av_dump_format(contextVk, 0, urlVk, 1);
        ret = avformat_write_header(contextVk, NULL);
        if (ret < 0) {
            qDebug()<<"Error avformat_write_header vk \n"<<av_make_error(ret);
            return -8;
        }
    }
    return 0;
}

QImage CameraWorker::avFrame2QImage(AVFrame *frame)
{
    struct SwsContext *img_convert_ctx = NULL;
    img_convert_ctx = sws_getContext(frame->width, frame->height,
                                     (AVPixelFormat)frame->format, frame->width, frame->height,
                                     AV_PIX_FMT_RGB24, SWS_FAST_BILINEAR, NULL, NULL, NULL);//SWS_BICUBIC

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

QVector<uint8_t> CameraWorker::extraDataAnnexbToAvcc(uint8_t* data, int len){
    QVector<uint8_t> arrData;
    if(len < 5)
        return arrData;
    if(data[0] != 0 || data[1] != 0 || data[2] != 0 || data[3] != 1)
        return arrData;
    int countZero = 0;
    int startPPS = 0;
    int lenSPS = 0;

    for(int i = 4; i < len; i++){
        if(data[i] == 0)
            countZero++;
        else if(data[i] == 1 && countZero == 3){
            startPPS = i + 1;
            lenSPS = i - 7;
            break;
        }
        else
            countZero = 0;
    }

    if(lenSPS == 0)
        return arrData;

    countZero = 0;

    int lenPPS = 0;

    for(int i = startPPS; i < len; i++){
        if(data[i] == 0)
            countZero++;
        else if(data[i] == 1 && countZero == 3){
            return arrData;
        }
        else
            countZero = 0;
        lenPPS++;
    }

    if(lenPPS == 0)
        return arrData;

    arrData.append(1);
    arrData.append(data[5]);                //SPS[0][1]
    arrData.append(data[6]);                //SPS[0][2]
    arrData.append(data[7]);                //SPS[0][3]
    arrData.append(0xff);
    arrData.append(0xe1);
    arrData.append((lenSPS >> 8) & 0xff);   //lenght
    arrData.append(lenSPS & 0xff);          //SPS
    /*Fill SPS data*/
    for(int i = 4; i < lenSPS + 4; i++)
        arrData.append(data[i]);
    arrData.append(1);                      //start PPS
    arrData.append((lenPPS >> 8) & 0xff);   //lenght
    arrData.append(lenPPS & 0xff);          //PPS
    /*Fill PPS data*/
    for(int i = startPPS; i < lenPPS + startPPS; i++)
        arrData.append(data[i]);

    return arrData;
}

int CameraWorker::addNewVideoStream(AVFormatContext * context, AVCodecContext* fmtContext, int w, int h, int f, int id, bool isStream)
{
    AVStream* stream = avformat_new_stream(context, NULL);
    if (!stream) {
        fprintf(stderr, "Failed allocating output stream\n");
        return -1;
    }

    AVCodecParameters* codecpar = avcodec_parameters_alloc();

    codecpar->width = w;
    codecpar->height = h;
    codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    codecpar->codec_tag = 0;
    codecpar->codec_id = AV_CODEC_ID_H264;
    codecpar->framerate = AVRational{f, 1};
    codecpar->bit_rate = (w * h * f) / 10;
    //codecpar->format = 12;

    if(isStream){
        QVector<uint8_t> vec = extraDataAnnexbToAvcc(fmtContext->extradata, fmtContext->extradata_size);
        codecpar->extradata_size = vec.length();
        codecpar->extradata = vec.data();
    }

    stream->id = id;
    stream->time_base = AVRational{1, 90000};//
    stream->avg_frame_rate = AVRational{f, 1};

    int ret = avcodec_parameters_copy(stream->codecpar, codecpar);
    if (ret < 0) {
        qDebug()<<"Failed to copy codec parameters\n"<<ret;
        return -1;
    }
    return 0;
}

int CameraWorker::addNewAudioStream(AVFormatContext * context, int id)
{
    AVStream* stream = avformat_new_stream(context, NULL);
    if (!stream) {
        qDebug()<<"Failed allocating output stream\n";
        return -1;
    }

    AVCodecParameters* codecpar = avcodec_parameters_alloc();

    codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
    codecpar->codec_tag = 0;
    codecpar->codec_id = AV_CODEC_ID_AAC;
    codecpar->ch_layout = AV_CHANNEL_LAYOUT_MONO;
    codecpar->ch_layout.nb_channels = 1;
    codecpar->sample_rate = 44100;
    codecpar->bit_rate = 128000;
    codecpar->format = AV_SAMPLE_FMT_FLTP;
    codecpar->profile = 1;

    codecpar->frame_size = 7168;

    stream->id = id;
    stream->time_base = AVRational{1, 90000};//

    codecpar->extradata_size = 2;
    codecpar->extradata = (uint8_t*)malloc(2);
    uint8_t *pExtradata = codecpar->extradata;
    pExtradata[0] = 0x12;
    pExtradata[1] = 0x8;

    int ret = avcodec_parameters_copy(stream->codecpar, codecpar);
    if (ret < 0) {
        qDebug()<<"Failed to copy codec parameters\n"<<ret;
        return -1;
    }
    return 0;
}

int CameraWorker::init_filter(FilteringContext *fctx, AVCodecContext *dec_ctx, AVCodecContext *enc_ctx, AVPixelFormat fmt, const char *filter_spec)
{
    char args[512];
    int ret = 0;
    const AVFilter *buffersrc = NULL;
    const AVFilter *buffersink = NULL;
    AVFilterContext *buffersrc_ctx = NULL;
    AVFilterContext *buffersink_ctx = NULL;
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    AVFilterGraph *filter_graph = avfilter_graph_alloc();

    if (!outputs || !inputs || !filter_graph) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
        buffersrc = avfilter_get_by_name("buffer");
        buffersink = avfilter_get_by_name("buffersink");
        if (!buffersrc || !buffersink) {
            av_log(NULL, AV_LOG_ERROR, "filtering source or sink element not found\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }

        snprintf(args, sizeof(args),
                 "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
                 dec_ctx->width, dec_ctx->height, fmt,//dec_ctx->pix_fmt,
                 dec_ctx->pkt_timebase.num, dec_ctx->pkt_timebase.den,
                 dec_ctx->sample_aspect_ratio.num,
                 dec_ctx->sample_aspect_ratio.den);

        ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                           args, NULL, filter_graph);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
            goto end;
        }

        buffersink_ctx = avfilter_graph_alloc_filter(filter_graph, buffersink, "out");
        if (!buffersink_ctx) {
            av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
            ret = AVERROR(ENOMEM);
            goto end;
        }

        ret = av_opt_set_bin(buffersink_ctx, "pix_fmts",
                             (uint8_t*)&enc_ctx->pix_fmt, sizeof(enc_ctx->pix_fmt),
                             AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot set output pixel format\n");
            goto end;
        }

        ret = avfilter_init_dict(buffersink_ctx, NULL);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot initialize buffer sink\n");
            goto end;
        }
    } else if (dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
        char buf[64];
        buffersrc = avfilter_get_by_name("abuffer");
        buffersink = avfilter_get_by_name("abuffersink");
        if (!buffersrc || !buffersink) {
            av_log(NULL, AV_LOG_ERROR, "filtering source or sink element not found\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }

        if (dec_ctx->ch_layout.order == AV_CHANNEL_ORDER_UNSPEC)
            av_channel_layout_default(&dec_ctx->ch_layout, dec_ctx->ch_layout.nb_channels);
        av_channel_layout_describe(&dec_ctx->ch_layout, buf, sizeof(buf));
        snprintf(args, sizeof(args),
                 "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=%s",
                 dec_ctx->pkt_timebase.num, dec_ctx->pkt_timebase.den, dec_ctx->sample_rate,
                 av_get_sample_fmt_name(dec_ctx->sample_fmt),
                 buf);
        ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                           args, NULL, filter_graph);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer source\n");
            goto end;
        }

        buffersink_ctx = avfilter_graph_alloc_filter(filter_graph, buffersink, "out");
        if (!buffersink_ctx) {
            av_log(NULL, AV_LOG_ERROR, "Cannot create audio buffer sink\n");
            ret = AVERROR(ENOMEM);
            goto end;
        }

        ret = av_opt_set_bin(buffersink_ctx, "sample_fmts",
                             (uint8_t*)&enc_ctx->sample_fmt, sizeof(enc_ctx->sample_fmt),
                             AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot set output sample format\n");
            goto end;
        }

        av_channel_layout_describe(&enc_ctx->ch_layout, buf, sizeof(buf));
        ret = av_opt_set(buffersink_ctx, "ch_layouts",
                         buf, AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot set output channel layout\n");
            goto end;
        }

        ret = av_opt_set_bin(buffersink_ctx, "sample_rates",
                             (uint8_t*)&enc_ctx->sample_rate, sizeof(enc_ctx->sample_rate),
                             AV_OPT_SEARCH_CHILDREN);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot set output sample rate\n");
            goto end;
        }

        if (enc_ctx->frame_size > 0)
            av_buffersink_set_frame_size(buffersink_ctx, enc_ctx->frame_size);

        ret = avfilter_init_dict(buffersink_ctx, NULL);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot initialize audio buffer sink\n");
            goto end;
        }
    } else {
        ret = AVERROR_UNKNOWN;
        goto end;
    }

    /* Endpoints for the filter graph. */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

    if (!outputs->name || !inputs->name) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    if ((ret = avfilter_graph_parse_ptr(filter_graph, filter_spec,
                                        &inputs, &outputs, NULL)) < 0)
        goto end;

    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
        goto end;

    /* Fill FilteringContext */
    fctx->buffersrc_ctx = buffersrc_ctx;
    fctx->buffersink_ctx = buffersink_ctx;
    fctx->filter_graph = filter_graph;

end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    return ret;
}

int CameraWorker::initFilter(AVCodecContext *dec_ctx, AVCodecContext *enc_ctx, AVPixelFormat fmt)
{
    const char *filter_spec;

    QString filter = "drawbox=%1:ih-%2-3*%1:(iw-%1*10)*0.45+%1*3:%2+2*%1:blue@0.5:t=fill, "
                     "drawbox=%1*2 :ih-%2-2*%1: (iw-%1*10)*0.35: %2:black:t=fill, "
                     "drawtext=fontsize=%2/2.5:fontcolor=White:fontfile=calibrib.ttf:textfile=name_blue.txt:x=%1*2+(%2-th)/2: y=h-%2-2*%1+(%2-th)/2: reload=1: line_spacing=5, "
                     "drawbox=%1*3+(iw-%1*10)*0.35: ih-%2-2*%1: (iw-%1*10)/10: %2:black:t=fill, "
                     "drawbox=%1*2+(iw-%1*4)*4.5/10: ih-%2-2*%1: (iw-%1*4)/10: %2:black:t=fill, "
                     "drawtext=fontsize=%2*4/5:fontcolor=White:fontfile=calibrib.ttf:textfile=rate_blue.txt:x=%1*3+(w-%1*10)*0.35+((w-%1*10)/10-tw)/2: y=h-%2-2*%1+(%2-th)/2: reload=1: shadowcolor=blue:shadowx=4: shadowy=4, "
                     "drawtext=fontsize=%2*4/5:fontcolor=LightGreen:fontfile=calibrib.ttf:textfile=time.txt:x=%1*2+(w-%1*4)*4.5/10+((w-%1*4)/10-tw)/2 :y=h-%2-2*%1+(%2-th)/2 :reload=1, "
                     "drawbox=iw-(iw-%1*10)*0.45-%1*3-%1: ih-%2-3*%1: (iw-%1*10)*0.45+%1*3: %2+2*%1:red@0.5:t=fill, "
                     "drawbox=iw-(iw-%1*10)*0.45-%1*3: ih-%2-2*%1: (iw-%1*10)/10: %2: black:t=fill, "
                     "drawtext=fontsize=%2*4/5:fontcolor=white:fontfile=calibrib.ttf:textfile=rate_red.txt: x=w-(w-%1*10)*0.45-%1*3+((w-%1*10)/10-tw)/2 :y=h-%2-2*%1+(%2-th)/2 :reload=1: shadowcolor=red: shadowx=4: shadowy=4, "
                     "drawbox=iw-w-%1*2 :ih-%2-2*%1: (iw-%1*10)*0.35: %2:black:t=fill, "
                     "drawtext=fontsize=%2/2.5:fontcolor=white:fontfile=calibrib.ttf:textfile=name_red.txt: x=w-(w-%1*10)*0.35-%1*2+(%2-th)/2: y=h-%2-2*%1+(%2-th)/2: reload=1: line_spacing=5";

    QString f = filter.arg(20).arg(100);

    std::string str = f.toStdString();
    filter_spec = str.c_str();

    filter_ctx = (FilteringContext*)av_malloc(sizeof(*filter_ctx));
    filter_ctx->buffersrc_ctx  = NULL;
    filter_ctx->buffersink_ctx = NULL;
    filter_ctx->filter_graph   = NULL;
    int ret = init_filter(filter_ctx, dec_ctx, enc_ctx, fmt, filter_spec);
    if (ret){
        enabledFilter = false;
        return -1;
    }

    filter_ctx->enc_pkt = av_packet_alloc();
    if (!filter_ctx->enc_pkt){
        enabledFilter = false;
        return -1;
    }

    filter_ctx->filtered_frame = av_frame_alloc();
    if (!filter_ctx->filtered_frame){
        enabledFilter = false;
        av_free(filter_ctx);
        return -1;
    }
    enabledFilter = true;
    return 0;
}

int CameraWorker::filter_encode_write_frame(AVFrame *frame)
{
    FilteringContext *filter = filter_ctx;
    int ret;
    /* push the decoded frame into the filtergraph */
    ret = av_buffersrc_add_frame_flags(filter->buffersrc_ctx,
                                       frame, 0);
    if (ret < 0) {
        qDebug()<<"Error while feeding the filtergraph";
        return ret;
    }

    /* pull filtered frames from the filtergraph */
    while (1) {
        ret = av_buffersink_get_frame(filter->buffersink_ctx,
                                      filter->filtered_frame);
        if (ret < 0) {
            /* if no more frames for output - returns AVERROR(EAGAIN)
             * if flushed and no more frames for output - returns AVERROR_EOF
             * rewrite retcode to 0 to show it as normal procedure completion
             */
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                ret = 0;
            else
                qDebug()<<QString::fromUtf8(_av_err2str(ret))<<ret;
            break;
        }
        filter->filtered_frame->time_base = av_buffersink_get_time_base(filter->buffersink_ctx);;
        filter->filtered_frame->pict_type = AV_PICTURE_TYPE_NONE;
    }

    return ret;
}

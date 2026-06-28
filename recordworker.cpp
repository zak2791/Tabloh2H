#include "recordworker.h"
#include "qdebug.h"

static const char* av_make_error(int errnum){
    static char str[AV_ERROR_MAX_STRING_SIZE];
    memset(str, 0, sizeof(str));
    return av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, errnum);
}

RecordWorker::RecordWorker(QString u, QMap<int, QList<int>> cams, QList<packet *> p, QObject* parent) : QObject(parent) {
    url = u;
    qDebug()<<"RecordWorker"<<u<<cams;
    params = cams;
    //firstVideo3packet = p;
    firstPackets = p;
}

RecordWorker::~RecordWorker(){
    emit stopped();
    qDebug()<<"~stop record";
    qDebug()<<"av_write_trailer = "<<av_write_trailer(outputContext);
    avformat_free_context(outputContext);

}



void RecordWorker::start(){
    qDebug()<<url;
    QByteArray ba = url.toUtf8();//toLatin1();
    const char* file = ba.constData();
    avformat_alloc_output_context2(&outputContext, NULL, "mp4", file);
    if (!outputContext) {
        qDebug()<<"Could not deduce output format from file extension: using MPEG.";
    }

    addNewAudioStream(outputContext);

    QList<int> list = params.value(1);
    qDebug()<<"list = "<<list;
    int numTracks = 0;
    if(!list.isEmpty()){
        addNewVideoStream(outputContext, list, ++numTracks);
        tracks.insert(1, numTracks);
    }
    list = params.value(2);
    if(!list.isEmpty()){
        addNewVideoStream(outputContext, list, ++numTracks);
        tracks.insert(2, numTracks);
    }
    list = params.value(3);
    if(!list.isEmpty()){
        addNewVideoStream(outputContext, list, ++numTracks);
        tracks.insert(3, numTracks);
    }
    if (!(outputContext->oformat->flags & AVFMT_NOFILE)) {
        int ret = avio_open(&outputContext->pb, file, AVIO_FLAG_WRITE);
        if (ret < 0) {
            qDebug()<<"Could not open output file '%s'"<<file;
        }
    }
    av_dump_format(outputContext, 0, file, 1);
    int ret = avformat_write_header(outputContext, NULL);
    if (ret < 0) {
        qDebug()<<"Error avformat_write_header\n"<<av_make_error(ret);
    }
    emit started();
}

void RecordWorker::setAvailableCams(){

}

void RecordWorker::packetHandler(packet p, int track){
    qDebug()<<"packetHandler "<<p.data.size()<<track<<tracks.value(track)<<tracks;
    QByteArray ba;

    if(track > 0){
        // if(firstPackets.at(tracks.value(track)) != NULL){
        //     qDebug()<<"firstPackets"<<(*firstPackets.at(tracks.value(track))).pts;
        //     (*firstPackets.at(tracks.value(track))).pts = p.pts;
        //     ba = (*firstPackets.at(tracks.value(track))).data;
        //     firstPackets[tracks.value(track)] = NULL;
        // }
        if(firstPackets.at(track - 1) != NULL){
            //qDebug()<<"firstPackets"<<(*firstPackets.at(tracks.value(track))).pts;
            (*firstPackets.at(track - 1)).pts = p.pts;
            ba = (*firstPackets.at(track - 1)).data;
            firstPackets[track - 1] = NULL;
        }
        else
            ba = p.data;
    }
    else
        ba = p.data;
    if(track > 0){
        int sizePacket = p.data.size();
        AVPacket *pPacket = av_packet_alloc();

        uint8_t* data = new uint8_t[sizePacket];
        memcpy(data, ba, sizePacket);
        pPacket->data = data;
        pPacket->size = sizePacket;
        pPacket->flags = p.flags;

        if(track == 1){
            if(startPtsVideo1 == -1){
                startPtsVideo1 = p.pts;
                qDebug()<<"startPtsVideo1";
            }
            pPacket->pts = (p.pts - startPtsVideo1) * 9 / 100;
        }
        else if(track == 2){
            if(startPtsVideo2 == -1){
                startPtsVideo2 = p.pts;
                qDebug()<<"startPtsVideo2";
            }
            pPacket->pts = (p.pts - startPtsVideo2) * 9 / 100;
        }
        else if(track == 3){
            if(startPtsVideo3 == -1){
                startPtsVideo3 = p.pts;
                qDebug()<<"startPtsVideo3";
            }
            pPacket->pts = (p.pts - startPtsVideo3) * 9 / 100;
        }

        pPacket->dts = pPacket->pts;

        pPacket->stream_index = tracks.value(track);
        qDebug()<<"pPacket->stream_index = "<<tracks.value(track);

        qDebug()<<av_interleaved_write_frame(outputContext, pPacket);
        av_packet_unref(pPacket);
        delete[] data;
    }
    else{
        const int l = p.data.size();
        AVPacket *pPacket = av_packet_alloc();
        uint8_t* data = new uint8_t[l];
        memcpy(data, ba, l);
        pPacket->data = data;
        pPacket->size = l;
        pPacket->stream_index = 0;
        pPacket->time_base = AVRational{0, 1};
        if(startPtsAudio == -1){
            startPtsAudio = p.pts;
            qDebug()<<"startPtsAudio";
        }
        pPacket->pts = (p.pts - startPtsAudio) / 10000 * 441;
        pPacket->dts = pPacket->pts;

        qDebug()<<"pts audio = "<<pPacket->pts;

        qDebug()<<"av_write_frame audio = "<<av_interleaved_write_frame(outputContext, pPacket);

        av_packet_free(&pPacket);
        delete[] data;
    }

}

int RecordWorker::addNewVideoStream(AVFormatContext * context, QList<int> parameters, int track)
{
    AVStream* stream = avformat_new_stream(context, NULL);
    if (!stream) {
        qDebug()<<"Failed allocating output stream";
        return -1;
    }

    AVCodecParameters* codecpar = avcodec_parameters_alloc();

    codecpar->width = parameters.at(0);
    codecpar->height = parameters.at(1);
    codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    codecpar->codec_tag = 0;
    codecpar->codec_id = AV_CODEC_ID_H264;
    codecpar->framerate = AVRational{parameters.at(2), 1};
    codecpar->bit_rate = (parameters.at(0) * parameters.at(1) * parameters.at(2)) / 10;
    //codecpar->format = 12;


    stream->id = track;//parameters.at(3);
    stream->time_base = AVRational{1, 90000};//
    stream->avg_frame_rate = AVRational{parameters.at(2), 1};

    int ret = avcodec_parameters_copy(stream->codecpar, codecpar);
    if (ret < 0) {
        qDebug()<<"Failed to copy codec parameters\n"<<ret;
        qDebug()<<av_make_error(ret);
        //goto end;
        return -1;
    }
    return 0;
}

int RecordWorker::addNewAudioStream(AVFormatContext * context)
{
    AVStream* stream = avformat_new_stream(context, NULL);
    if (!stream) {
        fprintf(stderr, "Failed allocating output stream\n");
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

    stream->id = 0;
    stream->time_base = AVRational{1, 90000};//

    codecpar->extradata_size = 2;
    codecpar->extradata = (uint8_t*)malloc(2);
    uint8_t *pExtradata = codecpar->extradata;
    pExtradata[0] = 0x12;
    pExtradata[1] = 0x8;

    int ret = avcodec_parameters_copy(stream->codecpar, codecpar);
    if (ret < 0) {
        qDebug()<<"Failed to copy codec parameters\n"<<ret;
        qDebug()<<av_make_error(ret);
        //goto end;
        return -1;
    }
    return 0;
}

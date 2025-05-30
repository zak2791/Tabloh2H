#include "player.h"

#include <QCamera>
#include <QMediaDevices>
#include <QAudioDevice>

#include "qprocess.h"
#include <QDebug>

#include <QFile>

Player::Player(QString file, QObject *parent) : QObject(parent){


}

Player::~Player()
{

}

QStringList Player::getListWebCams()
{
    QStringList listWebCam;
    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    for (const QCameraDevice &cameraDevice : cameras) {
        if(cameras.count() != 0){
            QList<QList<int>> list = getListParamWebCam(cameraDevice.description());
            if(list.count() != 0)
                listWebCam<<cameraDevice.description();
        }
    }
    return listWebCam;
}

QStringList Player::getListSoundDevices()
{
    QStringList listSoundDevices;
    const QList<QAudioDevice> audio = QMediaDevices::audioInputs();
    for (const QAudioDevice &audioDevices : audio)
        if(audio.count() != 0)
                listSoundDevices<<audioDevices.description();
    return listSoundDevices;
}

QList<QList<int>> Player::getListParamWebCam(QString text)
{
    QList<QList<int>> lParam;
    QCamera* cam;

    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    for (const QCameraDevice &cameraDevice : cameras) {
        if (cameraDevice.description() == text){
            //cam = new QCamera(cameraDevice);
            foreach(auto each, cameraDevice.videoFormats()){
                qDebug()<<each.pixelFormat()<<each.maxFrameRate()<<each.resolution();
                QList<int> par;
                par.append(each.maxFrameRate());
                par.append(each.resolution().rwidth());
                par.append(each.resolution().rheight());
                if(each.pixelFormat() ==  QVideoFrameFormat::Format_Jpeg)
                    lParam.append(par);
            }
            break;
        }
    }

    // if(cam->isAvailable())
    //     cam->start();

    // QList<QCameraViewfinderSettings> ViewSets = cam->supportedViewfinderSettings();
    // foreach (QCameraViewfinderSettings ViewSet, ViewSets) {
    //     QList<int> par;
    //     par.append(ViewSet.maximumFrameRate());
    //     par.append(ViewSet.resolution().rwidth());
    //     par.append(ViewSet.resolution().rheight());
    //     lParam.append(par);
    // }
    //QList<QCameraFormat> lFormat = cam->cameraDevice().videoFormats();
    // foreach(auto each, lFormat){
    //     qDebug()<<each.pixelFormat()<<each.maxFrameRate()<<each.resolution();
    //     QList<int> par;
    //     par.append(each.maxFrameRate());
    //     par.append(each.resolution().rwidth());
    //     par.append(each.resolution().rheight());
    //     if(each.pixelFormat() ==  QVideoFrameFormat::Format_Jpeg)
    //         lParam.append(par);
    // }
    //cam->stop();

    return lParam;
}

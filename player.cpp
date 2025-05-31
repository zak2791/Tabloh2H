#include "player.h"

#include <QCamera>
#include <QMediaDevices>
#include <QAudioDevice>

#include <QDebug>


Player::Player(QObject *parent) : QObject(parent){

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

    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    for (const QCameraDevice &cameraDevice : cameras) {
        if (cameraDevice.description() == text){
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

    return lParam;
}

#include "settingswebcamera.h"
#include "qcameradevice.h"
#include "ui_settingswebcamera.h"

#include <QMessageBox>

SettingsWebCamera::SettingsWebCamera(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsWebCamera)
{
    ui->setupUi(this);

    m_camera = nullptr;
    m_captureSession.setVideoOutput(ui->video);
    procSettingsCamera = new QProcess(this);
    procSettingsCamera->setProgram("ffmpeg");
}

SettingsWebCamera::~SettingsWebCamera()
{
    delete ui;
}

void SettingsWebCamera::setWebCamera(QString cam)
{
    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    for (const QCameraDevice &cameraDevice : cameras){
        if(cameraDevice.description() == cam){
            procSettingsCamera->setArguments({"-f", "dshow", "-show_video_device_dialog", "true", "-i", "video=" + cam});
            setCamera(cameraDevice);
        }
    }
}

void SettingsWebCamera::closeEvent(QCloseEvent *)
{
    if(procSettingsCamera->isOpen())
        procSettingsCamera->close();
    if(m_camera != nullptr){
        delete m_camera;
        m_camera = nullptr;
    }
}

void SettingsWebCamera::setCamera(const QCameraDevice &cameraDevice)
{
    m_camera = new QCamera(cameraDevice);
    m_captureSession.setCamera(m_camera);
    connect(m_camera, &QCamera::errorOccurred, this, &SettingsWebCamera::displayCameraError);
    m_camera->start();
    procSettingsCamera->start();
}

void SettingsWebCamera::displayCameraError()
{
    if (m_camera->error() != QCamera::NoError)
        QMessageBox::warning(this, tr("Camera Error"), m_camera->errorString());
}

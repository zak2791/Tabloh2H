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
        if(cameraDevice.id() == cam){
            QProcess procNameCamera;
            procNameCamera.setProgram("ffmpeg");
            procNameCamera.setArguments({"-hide_banner", "-f", "dshow", "-list_devices", "true", "-i", "dummy"});
            procNameCamera.setReadChannel(QProcess::StandardError);
            procNameCamera.start();
            procNameCamera.waitForFinished();
            QString camera = "";
            while(true){
                QByteArray ba = procNameCamera.readLine();
                if(ba.size() == 0)
                    break;

                QString fromFFmpeg = QString::fromLocal8Bit(ba).trimmed();
                QString id = cam;
                int firstIndex = cam.indexOf('#');
                int lastIndex = cam.lastIndexOf('#');
                id = id.mid(firstIndex,  lastIndex - firstIndex);
                fromFFmpeg = fromFFmpeg.removeLast();
                if(ba.contains(id.toUtf8())){
                    firstIndex = ba.indexOf("\"");
                    lastIndex = ba.lastIndexOf("\"");
                    ba = ba.mid(firstIndex + 1,  lastIndex - firstIndex - 1);
                    camera = QString::fromUtf8(ba);
                    break;
                }
            }
            procNameCamera.close();
            if(camera == "")
                return;
            procSettingsCamera->setArguments({"-f", "dshow", "-show_video_device_dialog", "true", "-i", "video=" + camera});
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

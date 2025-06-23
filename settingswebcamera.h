#ifndef SETTINGSWEBCAMERA_H
#define SETTINGSWEBCAMERA_H

#include "qmediadevices.h"
#include "qprocess.h"
#include <QDialog>

#include <QCamera>
#include <QMediaCaptureSession>

namespace Ui {
class SettingsWebCamera;
}

class SettingsWebCamera : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWebCamera(QWidget *parent = nullptr);
    ~SettingsWebCamera();

    void setWebCamera(QString);

private:
    Ui::SettingsWebCamera *ui;
    QMediaDevices m_devices;
    QMediaCaptureSession m_captureSession;
    QCamera* m_camera;
    QProcess* procSettingsCamera;

    virtual void closeEvent(QCloseEvent*);

private slots:
    void setCamera(const QCameraDevice &cameraDevice);
    void displayCameraError();
};

#endif // SETTINGSWEBCAMERA_H

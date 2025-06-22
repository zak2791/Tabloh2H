#ifndef SETTINGSWEBCAMERA_H
#define SETTINGSWEBCAMERA_H

#include <QDialog>

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
};

#endif // SETTINGSWEBCAMERA_H

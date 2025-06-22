#include "settingswebcamera.h"
#include "ui_settingswebcamera.h"

SettingsWebCamera::SettingsWebCamera(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsWebCamera)
{
    ui->setupUi(this);
}

SettingsWebCamera::~SettingsWebCamera()
{
    qDebug()<<"~SettingsWebCamera";
    delete ui;
}

void SettingsWebCamera::setWebCamera(QString cam)
{

}

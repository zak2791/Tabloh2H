#ifndef SETTINGSVIDEOREPLAY_H
#define SETTINGSVIDEOREPLAY_H

#include "cameraconnection.h"
#include "qcombobox.h"
#include "qsettings.h"
#include "settingswebcamera.h"
#include <QDialog>

namespace Ui {
class SettingsVideoReplay;
}

class SettingsVideoReplay : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsVideoReplay(QWidget *parent = nullptr);
    ~SettingsVideoReplay();

private:
    Ui::SettingsVideoReplay *ui;

    QWidget* control;
    QSettings* settings;
    CameraConnection* camConn;
    QString cam1Url = "";
    QString cam2Url = "";
    QString cam3Url = "";

    QString webCam1;
    QString paramWebCam1;
    QString webCam2;
    QString paramWebCam2;
    QString sound;

    SettingsWebCamera* sWeb;

    virtual void showEvent(QShowEvent *);

    void closeEvent(QCloseEvent *);

    QList<QStringList> getListWebCams();
    QStringList getListSoundDevices();
    QList<QList<int> > getListParamWebCam(QString text);
    QString getIdWebCam(QString);

    void SetComboBoxItemEnabled(QComboBox *comboBox, int index, bool enabled);
signals:
    void sigShowReplayOnTv(bool);
    void sigShowControlUsb(bool);

private slots:
    void autoCamera(bool state);
    void setCamera(QString ip);
    void setCam(QString text);
    void refreshWebCam();
    void setParamWebCam1(int);
    void setParamWebCam2(int);
    void selectWebCam1(int);
    void selectWebCam2(int);
    void setCamVk(bool);

};

#endif // SETTINGSVIDEOREPLAY_H

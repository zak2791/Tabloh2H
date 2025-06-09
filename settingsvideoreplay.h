#ifndef SETTINGSVIDEOREPLAY_H
#define SETTINGSVIDEOREPLAY_H

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


signals:
    void sigShowReplayOnTv(bool);

};

#endif // SETTINGSVIDEOREPLAY_H

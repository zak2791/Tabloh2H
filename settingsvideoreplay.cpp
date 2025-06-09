#include "settingsvideoreplay.h"
#include "ui_settingsvideoreplay.h"

SettingsVideoReplay::SettingsVideoReplay(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsVideoReplay)
{
    ui->setupUi(this);

    connect(ui->cbShowOnTv, &QCheckBox::clicked, this, &SettingsVideoReplay::sigShowReplayOnTv);

}

SettingsVideoReplay::~SettingsVideoReplay()
{
    delete ui;
}

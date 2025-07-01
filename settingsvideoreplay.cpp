#include "settingsvideoreplay.h"
#include "qaudiodevice.h"
#include "qcameradevice.h"
#include "qstandarditemmodel.h"
#include "ui_settingsvideoreplay.h"
#include "videoreplaycontrol.h"
#include <QMediaDevices>

SettingsVideoReplay::SettingsVideoReplay(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsVideoReplay)
{
    ui->setupUi(this);
    control = parent;

    settings = new QSettings("settings.ini", QSettings::IniFormat, this);

    connect(ui->cbShowOnTv, &QCheckBox::clicked, this, &SettingsVideoReplay::sigShowReplayOnTv);
    connect(ui->cbAutoCam1, &QCheckBox::toggled, this, &SettingsVideoReplay::autoCamera);
    connect(ui->cbAutoCam2, &QCheckBox::toggled, this, &SettingsVideoReplay::autoCamera);
    connect(ui->cbAutoCam3, &QCheckBox::toggled, this, &SettingsVideoReplay::autoCamera);
    connect(ui->leCam1, &QLineEdit::textEdited, this, &SettingsVideoReplay::setCam);
    connect(ui->leCam2, &QLineEdit::textEdited, this, &SettingsVideoReplay::setCam);
    connect(ui->leCam3, &QLineEdit::textEdited, this, &SettingsVideoReplay::setCam);
    connect(ui->btnRefresh, &QPushButton::clicked, this, &SettingsVideoReplay::refreshWebCam);

    connect(ui->cbSound, &QComboBox::currentTextChanged, static_cast<VideoReplayControl*>(control), &VideoReplayControl::setSound);

    settings->beginGroup("vk");
    QString urlVk = settings->value("url", "").toString();
    static_cast<VideoReplayControl*>(control)->setUrlVk(urlVk);
    ui->leUrl->setText(urlVk);

    QString keyVk       = settings->value("key", "").toString();
    static_cast<VideoReplayControl*>(control)->setKeyVk(keyVk);
    ui->leKey->setText(keyVk);

    int camToVk     = settings->value("cam", 0).toInt();
    static_cast<VideoReplayControl*>(control)->setCamToVk(camToVk);
    if(camToVk == 1)
        ui->rbVK1->setChecked(true);
    else if(camToVk == 2)
        ui->rbVK2->setChecked(true);
    else if(camToVk == 3)
        ui->rbVK3->setChecked(true);
    else
        ui->rbNoVK->setChecked(true);

    QString h = settings->value("heightPip", "120").toString();
    static_cast<VideoReplayControl*>(control)->setHeightPipVk(h);
    ui->sbHeight->setValue(h.toInt());

    settings->endGroup();

    connect(ui->sbHeight, &QSpinBox::valueChanged, this, [this](int value){
        static_cast<VideoReplayControl*>(control)->setHeightPipVk(QString::number(value));
        settings->beginGroup("vk");
        settings->setValue("heightPip", QString::number(value));
        settings->endGroup();
    });

    connect(ui->leUrl, &QLineEdit::editingFinished, this, [this](){
        static_cast<VideoReplayControl*>(control)->setUrlVk(ui->leUrl->text());
        settings->beginGroup("vk");
        settings->setValue("url", ui->leUrl->text());
        settings->endGroup();
    });

    connect(ui->leKey, &QLineEdit::editingFinished, this, [this](){
        static_cast<VideoReplayControl*>(control)->setKeyVk(ui->leKey->text());
        settings->beginGroup("vk");
        settings->setValue("key", ui->leKey->text());
        settings->endGroup();
    });

    connect(ui->rbVK1, &QRadioButton::clicked, this, &SettingsVideoReplay::setCamVk);
    connect(ui->rbVK2, &QRadioButton::clicked, this, &SettingsVideoReplay::setCamVk);
    connect(ui->rbVK3, &QRadioButton::clicked, this, &SettingsVideoReplay::setCamVk);
    connect(ui->rbNoVK, &QRadioButton::clicked, this, &SettingsVideoReplay::setCamVk);

    settings->beginGroup("webcam");
    webCam1 =   settings->value("cam1", "").toString();
    paramWebCam1 = settings->value("paramCam1", "").toString();
    webCam2 =   settings->value("cam2", "").toString();
    paramWebCam2 = settings->value("paramCam2", "").toString();
    sound = settings->value("sound", "").toString();
    settings->endGroup();

    settings->beginGroup("URL");
    cam1Url = settings->value("cam1", "").toString();
    static_cast<VideoReplayControl*>(control)->setCam1(cam1Url);
    ui->leCam1->setText(cam1Url);
    cam2Url = settings->value("cam2", "").toString();
    static_cast<VideoReplayControl*>(control)->setCam2(cam2Url);
    ui->leCam2->setText(cam2Url);
    cam3Url = settings->value("cam3", "").toString();
    static_cast<VideoReplayControl*>(control)->setCam3(cam3Url);
    ui->leCam3->setText(cam3Url);
    settings->endGroup();
    refreshWebCam();
    sWeb = new SettingsWebCamera(this);
    if(ui->cbWebCam1->currentText() == "")
        ui->btnSettingsWbCam1->setEnabled(false);
    if(ui->cbWebCam2->currentText()  == "")
        ui->btnSettingsWbCam2->setEnabled(false);
    connect(ui->btnSettingsWbCam1, &QPushButton::clicked, this, [this](){
        sWeb->setWebCamera(webCam1);
        sWeb->show();
    });
    connect(ui->btnSettingsWbCam2, &QPushButton::clicked, this, [this](){
        sWeb->setWebCamera(webCam2);
        sWeb->show();
    });
}

SettingsVideoReplay::~SettingsVideoReplay()
{
    delete ui;
}

void SettingsVideoReplay::showEvent(QShowEvent*){
    //refreshWebCam();
}

void SettingsVideoReplay::closeEvent(QCloseEvent *){
    if(ui->cbAutoCam1->isChecked())
        ui->cbAutoCam1->setChecked(false);
    if(ui->cbAutoCam2->isChecked())
        ui->cbAutoCam2->setChecked(false);
    if(ui->cbAutoCam3->isChecked())
        ui->cbAutoCam3->setChecked(false);
}

void SettingsVideoReplay::autoCamera(bool state){
    if(state){
        if(sender()->objectName() == "cbAutoCam1"){
            ui->cbAutoCam2->setEnabled(false);
            ui->cbAutoCam3->setEnabled(false);
            camConn = new CameraConnection(this);
            ui->leCam1->setText("");
            ui->leCam1->setStyleSheet("background-color: red");
        }
        else if(sender()->objectName() == "cbAutoCam2"){
            ui->cbAutoCam1->setEnabled(false);
            ui->cbAutoCam3->setEnabled(false);
            camConn = new CameraConnection(this, 2);
            ui->leCam2->setText("");
            ui->leCam2->setStyleSheet("background-color: red");
        }
        else{
            ui->cbAutoCam1->setEnabled(false);
            ui->cbAutoCam2->setEnabled(false);
            camConn = new CameraConnection(this, 3);
            ui->leCam3->setText("");
            ui->leCam3->setStyleSheet("background-color: red");
        }
        connect(camConn, SIGNAL(sigCamera(QString)), this, SLOT(setCamera(QString)));
    }
    else{
        if(sender()->objectName() == "cbAutoCam1"){
            ui->cbAutoCam2->setEnabled(true);
            ui->cbAutoCam3->setEnabled(true);
            ui->leCam1->setStyleSheet("background-color: white");
            settings->beginGroup("URL");
            ui->leCam1->setText(settings->value("cam1", "").toString());
            settings->endGroup();
        }
        else if(sender()->objectName() == "cbAutoCam2"){
            ui->cbAutoCam1->setEnabled(true);
            ui->cbAutoCam3->setEnabled(true);
            ui->leCam2->setStyleSheet("background-color: white");
            settings->beginGroup("URL");
            ui->leCam2->setText(settings->value("cam2", "").toString());
            settings->endGroup();
        }
        else{
            ui->cbAutoCam1->setEnabled(true);
            ui->cbAutoCam2->setEnabled(true);
            ui->leCam3->setStyleSheet("background-color: white");
            settings->beginGroup("URL");
            ui->leCam3->setText(settings->value("cam3", "").toString());
            settings->endGroup();
        }
        if(camConn){
            disconnect(camConn, SIGNAL(sigCamera(QString)), nullptr, nullptr);
            camConn->deleteLater();
        }
    }
}

void SettingsVideoReplay::setCamera(QString ip){

    if(ui->cbAutoCam1->isChecked()){
        cam1Url = "srt://" + ip + ":1111";
        settings->beginGroup("URL");
        settings->setValue("cam1", cam1Url);
        settings->endGroup();
        ui->cbAutoCam1->setChecked(false);
        static_cast<VideoReplayControl*>(control)->setCam1(cam1Url);
    }
    if(ui->cbAutoCam2->isChecked()){
        cam2Url = "srt://" + ip + ":2222";
        settings->beginGroup("URL");
        settings->setValue("cam2", cam2Url);
        settings->endGroup();
        ui->cbAutoCam2->setChecked(false);
        static_cast<VideoReplayControl*>(control)->setCam2(cam2Url);
    }
    if(ui->cbAutoCam3->isChecked()){
        cam3Url = "srt://" + ip + ":3333";
        settings->beginGroup("URL");
        settings->setValue("cam3", cam2Url);
        settings->endGroup();
        ui->cbAutoCam3->setChecked(false);
        static_cast<VideoReplayControl*>(control)->setCam3(cam3Url);
    }
}

void SettingsVideoReplay::setCam(QString text){
    if(sender()->objectName() == "leCam1"){
        cam1Url = text;
        settings->beginGroup("URL");
        settings->setValue("cam1", cam1Url);
        settings->endGroup();
        static_cast<VideoReplayControl*>(control)->setCam1(cam1Url);
    }else if(sender()->objectName() == "leCam2"){
        cam2Url = text;
        settings->beginGroup("URL");
        settings->setValue("cam2", cam2Url);
        settings->endGroup();
        static_cast<VideoReplayControl*>(control)->setCam2(cam2Url);
    }else{
        cam3Url = text;
        settings->beginGroup("URL");
        settings->setValue("cam3", cam3Url);
        settings->endGroup();
        static_cast<VideoReplayControl*>(control)->setCam3(cam3Url);
    }
}

QList<QStringList> SettingsVideoReplay::getListWebCams()
{
    QList<QStringList> listWebCam;
    int i = 0;
    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    for (const QCameraDevice &cameraDevice : cameras) {
        if(cameras.count() != 0){
            QList<QList<int>> list = getListParamWebCam(cameraDevice.id());
            if(list.count() != 0){
                QStringList list;
                list<<cameraDevice.description() + " " + QString::number(++i)<<cameraDevice.id();
                listWebCam<<list;
            }
        }
    }
    return listWebCam;
}

QStringList SettingsVideoReplay::getListSoundDevices()
{
    QStringList listSoundDevices;
    const QList<QAudioDevice> audio = QMediaDevices::audioInputs();
    for (const QAudioDevice &audioDevices : audio)
        if(audio.count() != 0)
            listSoundDevices<<audioDevices.description();
    return listSoundDevices;
}

QList<QList<int>> SettingsVideoReplay::getListParamWebCam(QString text)
{
    QList<QList<int>> lParam;
    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    for (const QCameraDevice &cameraDevice : cameras) {
        if (cameraDevice.id() == text){
            foreach(auto each, cameraDevice.videoFormats()){
                QList<int> par;
                par.append(each.maxFrameRate());
                par.append(each.resolution().rwidth());
                par.append(each.resolution().rheight());
                if(each.pixelFormat() ==  QVideoFrameFormat::Format_Jpeg &&
                    each.resolution().rwidth() <= 1920 &&
                    each.resolution().rwidth() >= 800 &&
                    each.maxFrameRate() >= 15)
                    lParam.append(par);
            }
            break;
        }
    }
    return lParam;
}

QString SettingsVideoReplay::getIdWebCam(QString cam)
{
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
    qDebug()<<"camera = "<<camera;
    return camera;

}

void SettingsVideoReplay::refreshWebCam()
{
    QList<QStringList> cams = getListWebCams();
    ui->cbWebCam1->disconnect();
    ui->cbWebCam2->disconnect();
    ui->cbParamWebCam1->disconnect();
    ui->cbParamWebCam2->disconnect();

    ui->cbSound->clear();
    ui->cbWebCam1->clear();
    ui->cbWebCam1->addItem("");
    ui->cbParamWebCam1->clear();

    foreach(auto each, cams){
        ui->cbWebCam1->addItem(each.at(0), each.at(1));
        qDebug()<<each.at(0)<<each.at(1);
    }

    ui->cbWebCam2->clear();
    ui->cbWebCam2->addItem("");
    ui->cbParamWebCam2->clear();

    foreach(auto each, cams)
        ui->cbWebCam2->addItem(each.at(0), each.at(1));

    ui->cbSound->addItems(getListSoundDevices());

    int index = ui->cbWebCam1->findData(webCam1);
    if(index > 0){

        ui->cbWebCam1->setCurrentIndex(index);
        static_cast<VideoReplayControl*>(control)->setWebCam1(getIdWebCam(webCam1));/////////////////////
        ui->btnSettingsWbCam1->setEnabled(true);
        QList<QList<int>> param = getListParamWebCam(webCam1);
        int count = 0;
        foreach(auto each, param){
            QString sParam = "fps = " + QString::number(each.at(0)) +
                             "resolution = " + QString::number(each.at(1)) +
                             "x" + QString::number(each.at(2));
            ui->cbParamWebCam1->addItem(sParam);
            ui->cbParamWebCam1->setItemData(count++, QVariant::fromValue(each));
        }
        index = ui->cbParamWebCam1->findText(paramWebCam1);
        if(index > 0)
            ui->cbParamWebCam1->setCurrentIndex(index);
        static_cast<VideoReplayControl*>(control)->setParamWebCam1(ui->cbParamWebCam1->currentData().value<QList<int>>());
        ui->leCam1->setText(ui->cbWebCam1->currentText());
        ui->leCam1->setEnabled(false);
        ui->cbAutoCam1->setEnabled(false);
    }

    connect(ui->cbWebCam1, &QComboBox::currentIndexChanged, this, &SettingsVideoReplay::selectWebCam1);
    connect(ui->cbParamWebCam1, QOverload<int>::of(&QComboBox::activated), this, &SettingsVideoReplay::setParamWebCam1);

    index = ui->cbWebCam2->findData(webCam2);
    if(index > 0){
        ui->cbWebCam2->setCurrentIndex(index);
        static_cast<VideoReplayControl*>(control)->setWebCam2(getIdWebCam(webCam2));
        ui->btnSettingsWbCam2->setEnabled(true);
        QList<QList<int>> param = getListParamWebCam(webCam2);
        int count = 0;
        foreach(auto each, param){
            QString sParam = "fps = " + QString::number(each.at(0)) +
                             "resolution = " + QString::number(each.at(1)) +
                             "x" + QString::number(each.at(2));
            ui->cbParamWebCam2->addItem(sParam);
            ui->cbParamWebCam2->setItemData(count++, QVariant::fromValue(each));
        }
        index = ui->cbParamWebCam2->findText(paramWebCam2);
        if(index > 0)
            ui->cbParamWebCam2->setCurrentIndex(index);
        static_cast<VideoReplayControl*>(control)->setParamWebCam2(ui->cbParamWebCam2->currentData().value<QList<int>>());
        ui->leCam2->setText(ui->cbWebCam2->currentText());
        ui->leCam2->setEnabled(false);
        ui->cbAutoCam2->setEnabled(false);
    }

    connect(ui->cbWebCam2, &QComboBox::currentIndexChanged, this, &SettingsVideoReplay::selectWebCam2);
    connect(ui->cbParamWebCam2, QOverload<int>::of(&QComboBox::activated), this, &SettingsVideoReplay::setParamWebCam2);

    index = ui->cbSound->findText(sound);
    if(index != -1)
        ui->cbSound->setCurrentIndex(index);

    index = ui->cbWebCam2->findData(webCam1);
    if(index > 0)
        SetComboBoxItemEnabled(ui->cbWebCam2 , index, false);

    index = ui->cbWebCam1->findData(webCam2);
    if(index > 0)
        SetComboBoxItemEnabled(ui->cbWebCam1 , index, false);


    connect(ui->cbWebCam1, &QComboBox::currentIndexChanged, this, [this](int index){
        for(int i = 1; i < ui->cbWebCam2->count(); i++)
            SetComboBoxItemEnabled(ui->cbWebCam2 , i, true);
        if(index != 0)
            SetComboBoxItemEnabled(ui->cbWebCam2, index, false);
    });
    connect(ui->cbWebCam2, &QComboBox::currentIndexChanged, this, [this](int index){
        for(int i = 1; i < ui->cbWebCam1->count(); i++)
            SetComboBoxItemEnabled(ui->cbWebCam1 , i, true);
        if(index != 0)
            SetComboBoxItemEnabled(ui->cbWebCam1, index, false);
    });

}

void SettingsVideoReplay::SetComboBoxItemEnabled(QComboBox * comboBox, int index, bool enabled)
{
    auto * model = qobject_cast<QStandardItemModel*>(comboBox->model());
    assert(model);
    if(!model) return;

    auto * item = model->item(index);
    assert(item);
    if(!item) return;
    item->setEnabled(enabled);
}

void SettingsVideoReplay::setParamWebCam1(int index)
{
    settings->beginGroup("webcam");
    if(index == -1){
        settings->setValue("cam1", "");
        settings->setValue("paramCam1", "");
    }
    else{
        settings->setValue("cam1", ui->cbWebCam1->currentData());
        settings->setValue("paramCam1", ui->cbParamWebCam1->currentText());
        QVariant variant = ui->cbParamWebCam1->itemData(index);
        QList<int> data = variant.value<QList<int>>();
        qDebug()<<"setParamWebCam1"<<data;
        static_cast<VideoReplayControl*>(control)->setParamWebCam1(data);
    }
    settings->endGroup();
}

void SettingsVideoReplay::setParamWebCam2(int index)
{   
    settings->beginGroup("webcam");
    if(index == -1){
        settings->setValue("cam2", "");
        settings->setValue("paramCam2", "");
    }
    else{
        settings->setValue("cam2", ui->cbWebCam2->currentData());
        settings->setValue("paramCam2", ui->cbParamWebCam2->currentText());
        QVariant variant = ui->cbParamWebCam2->itemData(index);
        QList<int> data = variant.value<QList<int>>();
        static_cast<VideoReplayControl*>(control)->setParamWebCam2(data);
    }
    settings->endGroup();
}

void SettingsVideoReplay::selectWebCam1(int index)
{
    ui->cbParamWebCam1->clear();
    if(index == 0){
        ui->btnSettingsWbCam1->setEnabled(false);
        setParamWebCam1(-1);
        ui->leCam1->setText(cam1Url);
        ui->leCam1->setEnabled(true);
        ui->cbAutoCam1->setEnabled(true);
        webCam1 = "";
        static_cast<VideoReplayControl*>(control)->setWebCam1(webCam1);
        for(int i = 1; i < ui->cbWebCam2->count(); i++)
            SetComboBoxItemEnabled(ui->cbWebCam2 , i, true);
        return;
    }
    if(ui->cbWebCam1->count() > 0){
        webCam1 = ui->cbWebCam1->currentData().toString();
        static_cast<VideoReplayControl*>(control)->setWebCam1(getIdWebCam(webCam1));
        QList<QList<int>> param = getListParamWebCam(webCam1);
        int count = 0;
        foreach(auto each, param){
            QString sParam = "fps = " + QString::number(each.at(0)) +
                             "resolution = " + QString::number(each.at(1)) +
                             "x" + QString::number(each.at(2));
            ui->cbParamWebCam1->addItem(sParam);
            ui->cbParamWebCam1->setItemData(count++, QVariant::fromValue(each));
        }
        ui->btnSettingsWbCam1->setEnabled(true);
        setParamWebCam1(ui->cbParamWebCam1->currentIndex());
        ui->leCam1->setText(ui->cbWebCam1->currentText());
        ui->leCam1->setEnabled(false);
        ui->cbAutoCam1->setEnabled(false);
    }
}

void SettingsVideoReplay::selectWebCam2(int index)
{
    ui->cbParamWebCam2->clear();
    if(index == 0){
        ui->btnSettingsWbCam2->setEnabled(false);
        setParamWebCam2(-1);
        ui->leCam2->setText(cam2Url);
        ui->leCam2->setEnabled(true);
        ui->cbAutoCam2->setEnabled(true);
        webCam2 = "";
        static_cast<VideoReplayControl*>(control)->setWebCam2(webCam2);
        for(int i = 1; i < ui->cbWebCam1->count(); i++)
            SetComboBoxItemEnabled(ui->cbWebCam1 , i, true);
        return;
    }
    if(ui->cbWebCam2->count() > 0){
        webCam2  = ui->cbWebCam2->currentData().toString();
        static_cast<VideoReplayControl*>(control)->setWebCam2(getIdWebCam(webCam2));
        QList<QList<int>> param = getListParamWebCam(webCam2);
        int count = 0;
        foreach(auto each, param){
            QString sParam = "fps = " + QString::number(each.at(0)) +
                             "resolution = " + QString::number(each.at(1)) +
                             "x" + QString::number(each.at(2));
            ui->cbParamWebCam2->addItem(sParam);
            ui->cbParamWebCam2->setItemData(count++, QVariant::fromValue(each));
        }
        ui->btnSettingsWbCam2->setEnabled(true);
        setParamWebCam2(ui->cbParamWebCam2->currentIndex());
        ui->leCam2->setText(ui->cbWebCam2->currentText());
        ui->leCam2->setEnabled(false);
        ui->cbAutoCam2->setEnabled(false);
    }
}

void SettingsVideoReplay::setCamVk(bool checked)
{
    if(checked){
        settings->beginGroup("vk");
        if(sender()->objectName() == "rbVK1"){
            settings->setValue("cam", 1);
            static_cast<VideoReplayControl*>(control)->setCamToVk(1);
        }
        else if(sender()->objectName() == "rbVK2"){
            settings->setValue("cam", 2);
            static_cast<VideoReplayControl*>(control)->setCamToVk(2);
        }
        else if(sender()->objectName() == "rbVK3"){
            settings->setValue("cam", 3);
            static_cast<VideoReplayControl*>(control)->setCamToVk(3);
        }
        else{
            settings->setValue("cam", 0);
            static_cast<VideoReplayControl*>(control)->setCamToVk(0);
        }
        settings->endGroup();
    }
}

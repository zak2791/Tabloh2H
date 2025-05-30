#include "mainwindow.h"
#include "pcscreen.h"
//#include <QRegExp>
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QCloseEvent>

#include <QDebug>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    lblStatus = new QLabel();
    statusBar()->addWidget(lblStatus);
    winSettings = ui->winSettings;
    winVideoSettings = ui->videoSettings;
    //tvSettings = ui->tvSettings;
    time = ui->setTime;
    actSbros = ui->actSbros;
    video = ui->video;
    no_video = ui->no_video;
    winHelp = ui->winHelp;

    fileSettings = "settings.ini";


    fileSportsmens = "";

    QAction* openFile = ui->openFile;
    connect(openFile, SIGNAL(triggered()), this, SLOT(openFile()));

    settings = new QSettings(fileSettings, QSettings::IniFormat, this);
    settings->beginGroup("files");
    lastDir = settings->value("lastDir", "").toString();
    QString lFiles = settings->value("listFiles", "").toString();


    lastFiles = ui->mFile->addMenu("Последние файлы");
    closeProg = ui->mFile->addAction("Выход");
    if(lFiles == "")
        lastFiles->setEnabled(false);
    else{
        listFiles = lFiles.split(";");
        foreach(QString str, listFiles){
            if(QFile::exists(str)){
                QAction* act = lastFiles->addAction(str);
                act->setCheckable(true);
                if(str == listFiles.at(0)){
                    act->setChecked(true);
                    fileSportsmens = str;
                }
                connect(act, SIGNAL(triggered()), SLOT(choiceFile()));
            }
            else{
                listFiles.removeOne(str);
            }
        }
        settings->setValue("listFiles", listFiles.join(";"));
    }
    settings->endGroup();

    no_video->setCheckable(true);
    no_video->setObjectName("0");
    video->setCheckable(true);
    video->setObjectName("1");

    connect(no_video, SIGNAL(triggered()), this, SLOT(Variant()));
    connect(video,    SIGNAL(triggered()), this, SLOT(Variant()));

    no_video->setChecked(true);

    hf = new HelpForm;
    connect(winHelp, SIGNAL(triggered()), hf, SLOT(show()));

    QDialog* frmVersion = new QDialog;
    frmVersion->setModal(true);
    uiVersion.setupUi(frmVersion);
    connect(ui->about, SIGNAL(triggered()), frmVersion, SLOT(show()));

    connect(ui->logoRus, SIGNAL(triggered(bool)), this, SLOT(selectLogo(bool)));
    connect(ui->logoEng, SIGNAL(triggered(bool)), this, SLOT(selectLogo(bool)));

    settings->beginGroup("logo");
    if(settings->value("logo", 1).toInt()){
        ui->logoRus->setChecked(true);
        ui->logoEng->setChecked(false);
    }
    else{
        ui->logoRus->setChecked(false);
        ui->logoEng->setChecked(true);
    }
    settings->endGroup();

    //dlg = new QDialog;
    //uiObs.setupUi(dlg);

    //connect(ui->setOBS, SIGNAL(triggered()), dlg, SLOT(show()));

    // settings->beginGroup("obs");
    // int obsPort = settings->value("port", 4455).toInt();
    // QString obsIpAddr = settings->value("ipAddr", "localhost").toString();
    // QString obsPassword = settings->value("password", "").toString();
    // bool turnObs = settings->value("turnObs", false).toBool();
    // settings->endGroup();

    // uiObs.IpAddress->setText(obsIpAddr);
    // uiObs.Password->setText(obsPassword);
    // uiObs.Port->setValue(obsPort);
    // uiObs.cbConnectToOBS->setChecked(turnObs);

    // connect(dlg, SIGNAL(accepted()), this, SLOT(slotAcceptSettingsObs()));
    // connect(dlg, SIGNAL(rejected()), this, SLOT(slotRejectSettingsObs()));

    registration = ui->registration;
    connect(registration, &QAction::triggered, this, &MainWindow::slotRegistration);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QProcess process(this);
    process.setProgram("getmac");
    process.start();
    process.waitForFinished (); // Ожидание завершения процесса запуска, тайм-аут 30 с, затем блокировка контакта
    QRegularExpression re("^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})|([0-9a-fA-F]{4}\\.[0-9a-fA-F]{4}\\.[0-9a-fA-F]{4})$");
    serialNumberMac = "";
    while(1){
        char buf[1024];
        qint64 lineLength = process.readLine(buf, sizeof(buf));
        if (lineLength == -1) {
            break;
        }
        QString s = QString::fromUtf8(buf);
        QList<QString> list = s.split(QRegularExpression("\\s+"));
        foreach(auto each, list){
            QRegularExpressionMatch match = re.match(each);
            if (match.hasMatch()) {
                serialNumberMac = each;
                break;
            }
        }
    }

    process.setProgram("wmic");
    process.setArguments({"bios", "get", "serialnumber"});
    process.start();
    process.waitForFinished (); // Ожидание завершения процесса запуска, тайм-аут 30 с, затем блокировка контакта
    QString s;
    while(true){
        s = QString::fromLocal8Bit(process.readLine());
        if(s.contains("SerialNumber"))
            break;
    }

    serialNumberBios = QString::fromLocal8Bit(process.readLine()).simplified();
    qDebug()<<serialNumberBios;
    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.beginGroup("code");
    QString codeBios(settings.value("codeBios", "").toString());
    QString codeMacAddr(settings.value("codeMacAddr", "").toString());
    settings.endGroup();

    if(codeBios != ""){
        readCode = codeBios;
        serialNumber = serialNumberBios;
    }
    else if(codeMacAddr != ""){
        readCode = codeMacAddr;
        serialNumber = serialNumberMac;
    }

    frmCode = new QDialog;

    ui_code.setupUi(frmCode);
    ui_code.leID->setText(serialNumberBios);
    frmCode->setModal(true);
    leId = ui_code.leID;
    connect(ui_code.rbBios, SIGNAL(toggled(bool)), this, SLOT(slotChangeId(bool)));

    QString code = calculateCode(serialNumber);

    //connect(centralWidget();

    if(code == readCode && code != ""){
        registration->setEnabled(false);
        //emit sigRegistration();
        //static_cast<PCScreen*>(centralWidget())->slotRegistration();

    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

// void MainWindow::slotAcceptSettingsObs(){
//     settings->beginGroup("obs");
//     settings->setValue("port", uiObs.Port->value());
//     settings->setValue("ipAddr", uiObs.IpAddress->text());
//     settings->setValue("password", uiObs.Password->text());
//     settings->setValue("turnObs", uiObs.cbConnectToOBS->isChecked());
//     settings->endGroup();
// }

// void MainWindow::slotRejectSettingsObs(){
//     settings->beginGroup("obs");
//     uiObs.Port->setValue(settings->value("port", 4455).toInt());
//     uiObs.IpAddress->setText(settings->value("ipAddr", "localhost").toString());
//     uiObs.Password->setText(settings->value("password", "").toString());
//     settings->endGroup();
// }

void MainWindow::slotChangeId(bool b)
{
    if(b)
        leId->setText(serialNumberBios);
    else
        leId->setText(serialNumberMac);
}

int MainWindow::func(int num){
    int dig = 0;
    QString str_num = QString::number(num);
    for(int i = 0; i < str_num.length(); i++){
        dig +=  QString(str_num.at(i)).toInt();
    }
    if(dig > 9){
        return func(dig);
    }else{
        return dig;
    }
}

QString MainWindow::calculateCode(QString serial)
{
    int lenString = serial.length();
    int myListStart[lenString];
    int myListEnd[lenString];
    for(int i=0; i < lenString; i++)
        myListStart[i] = serial.at(i).toLatin1();

    QString code = "";
    for(int i=0; i < lenString; i++){
        if(i < lenString - 1){
            myListEnd[i] = myListStart[i] + myListStart[i + 1];
        }else{
            myListEnd[i] = myListStart[i] + myListStart[0];
        }
        code = code + QString::number(func(myListEnd[i]));
    }
    return code;
}

void MainWindow::slotRegistration()
{
    qDebug()<<"slotReg";

    QString code;// = calculateCode(serialNumber);
    while(1){
        int ret = frmCode->exec();
        if(ret == 1){
            if(ui_code.rbBios->isChecked())
                code = calculateCode(serialNumberBios);
            else
                code = calculateCode(serialNumberMac);

            if(ui_code.leCode->text() == code){
                settings->beginGroup("code");
                if(ui_code.rbBios->isChecked()) {
                    settings->setValue("codeBios", code);
                    settings->setValue("codeMacAddr", "");
                }
                else {
                    settings->setValue("codeBios", "");
                    settings->setValue("codeMacAddr", code);
                }
                settings->endGroup();
                //tvScreen->removeLogo();
                registration->setEnabled(false);
                //emit sigRegistration();
                //static_cast<PCScreen*>(centralWidget())->slotRegistration();
                break;
            }else{
                //frmCode->leCode->setText("");
                //break;
            }
        }else{
            //delete frmCode;
            //tvScreen->removeLogo();
            break;
        }
    }
}

void MainWindow::Variant(){
    if(sender()->objectName() == "0"){
        if(no_video->isChecked() == true){
            video->setChecked(false);
            emit variant(0);
        }
        else
            no_video->setChecked(true);
    }
    else{
        if(video->isChecked() == true){
            no_video->setChecked(false);
            emit variant(1);
        }
        else
            video->setChecked(true);
    }

}

void MainWindow::selectLogo(bool checked)
{
    if(sender()->objectName() == "logoRus"){
        if(ui->logoEng->isChecked() == false){
            ui->logoRus->setChecked(true);
            return;
        }
        ui->logoEng->setChecked(false);
        emit sigLogo(true);
    }
    else{
        if(ui->logoRus->isChecked() == false){
            ui->logoEng->setChecked(true);
            return;
        }
        ui->logoRus->setChecked(false);
        emit sigLogo(false);
    }

    qDebug()<<sender()->objectName();
}

void MainWindow::closeEvent(QCloseEvent* e){
    qDebug()<<"closeMain";
    int ret = QMessageBox::question(this, tr("Выход"),
                                    tr("Вы уверены?"),
                                    QMessageBox::Yes | QMessageBox::No
                                    );
    if(ret == QMessageBox::Yes){
        //emit sigExit();


        PCScreen* ps = (PCScreen*)centralWidget();
        //delete ps;
        ps->close();
        e->accept();
        //qApp->exit(0);
    }
    else
        e->ignore();
}

void MainWindow::openFile(){
    QString file = QFileDialog::getOpenFileName(this, tr("Выберите файл"),  lastDir, tr("*.xlsx *.xls"));
    if(file == "")
        return;
    fileSportsmens = file;

    listFiles.removeOne(fileSportsmens);
    listFiles.prepend(fileSportsmens);

    foreach(QAction* a, lastFiles->findChildren<QAction*>())
        lastFiles->removeAction(a);

    foreach(QString str, listFiles){
        QAction* act = lastFiles->addAction(str);
        act->setCheckable(true);
        if(str == listFiles.at(0))
            act->setChecked(true);
        connect(act, SIGNAL(triggered()), SLOT(choiceFile()));
    }

    lastFiles->setEnabled(true);
    lastDir = file.remove(file.lastIndexOf('/'), 100);

    QSettings settings(fileSettings, QSettings::IniFormat);
    settings.beginGroup("files");
    settings.setValue("lastDir", lastDir);
    settings.setValue("listFiles", listFiles.join(";"));
    settings.endGroup();   
    settings.sync();

    emit newFile();
}

void MainWindow::choiceFile(){
    QAction* action = static_cast<QAction*>(sender());
    qDebug()<<action->text();
    action->setChecked(true);
    fileSportsmens = action->text();

    listFiles.removeOne(fileSportsmens);
    listFiles.prepend(fileSportsmens);

    foreach(QAction* a, lastFiles->findChildren<QAction*>())
        lastFiles->removeAction(a);

    foreach(QString str, listFiles){
        QAction* act = lastFiles->addAction(str);
        act->setCheckable(true);
        if(str == listFiles.at(0))
            act->setChecked(true);
        connect(act, SIGNAL(triggered()), SLOT(choiceFile()));
    }

    QSettings settings(fileSettings, QSettings::IniFormat);
    settings.beginGroup("files");
    settings.setValue("listFiles", listFiles.join(";"));
    settings.endGroup();
    settings.sync();

    emit newFile();
}

QString MainWindow::getFileSportsmens(void){
    return fileSportsmens;
}

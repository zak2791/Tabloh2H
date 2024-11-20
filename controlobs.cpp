 #include "controlobs.h"
#include <QApplication>
//#include <QDebug>

ControlObs::ControlObs(QString ipAddr, int p , QString passwd)
{
    f_Lib = new QLibrary;

    port = p;

    wIp = {new wchar_t[ipAddr.length()]};
    wPassw = {new wchar_t[passwd.length()]};

    ipAddr.toWCharArray(wIp);
    passwd.toWCharArray(wPassw);
    //qDebug()<<wIp<<wPassw;
}

ControlObs::~ControlObs()
{
    delete f_Lib;
    delete wIp;
    delete wPassw;
}

void ControlObs::doWork(const QString &parameter) {
    QString result = "ok";
    /* ... here is the expensive or blocking operation ... */

    try
    {
        if( !f_Lib->isLoaded() ) {
            f_Lib->setFileName(qApp->applicationDirPath() + "\\libConnectOBS\\libConnectOBS.dll");
            if( !f_Lib->load() ) {
                QString e_Error = "Ошибка при загрузке библиотеки интеграции с OBS Studio: " + f_Lib->errorString();
                throw std::runtime_error(e_Error.toStdString());
            }
        }
        typedef const wchar_t* (*StartRecordingFight)(const wchar_t* p_Host, unsigned int p_Port, const wchar_t* p_Password);
        StartRecordingFight a_StartRecordingFight = (StartRecordingFight)f_Lib->resolve("StartRecordingFight");
        if( !a_StartRecordingFight ) {
            QString e_Error = "Ошибка при поиске метода StartRecordingFight в библиотеке интеграции с OBS Studio: " + f_Lib->errorString();
            throw std::runtime_error(e_Error.toStdString());
        }

        const wchar_t* a_Result = a_StartRecordingFight(wIp,
                                                        port,
                                                        wPassw);
        QString str(QString::fromWCharArray(a_Result));

        if(!str.isEmpty())
        {
            throw std::runtime_error(str.toStdString());
        }
    }
    catch(const std::exception &e)
    {
        // QMessageBox msgBox(QMessageBox::Icon::Critical,
        //                    "Ошибка видеоповтора",
        //                    QString("Видеоповтор не работает из за возникновения ошибки:\r\n") + e.what());
        // msgBox.exec();
        result = QString("Видеоповтор не работает из за возникновения ошибки:\r\n") + e.what();
    }
    //return result;

    emit resultReady(result);
}


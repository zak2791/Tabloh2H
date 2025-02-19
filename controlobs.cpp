 #include "controlobs.h"
#include <QApplication>

ControlObs::ControlObs()
{
    f_Lib = new QLibrary;
}

ControlObs::~ControlObs()
{
    delete f_Lib;
}

void ControlObs::doWork(QString addr, int port, QString passw) {
    QString result = "ok";

    wchar_t wIp[addr.length() + 1]; // = {wchar_t[addr.length() + 1]};
    wIp[addr.length()] = 0;

    wchar_t wPassw[passw.length() + 1]; // = {wchar_t[passw.length() + 1]};
    wPassw[passw.length()] = 0;

    addr.toWCharArray(wIp);
    passw.toWCharArray(wPassw);

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
        result = QString("Видеоповтор не работает из за возникновения ошибки:\r\n") + e.what();
    }
    emit resultReady(result);
}


#pragma once
#include <QtWidgets> 
#include "First_display.h"

class JSTools : public QObject {
	Q_OBJECT
public:
	JSTools(QObject* pobj = 0) : QObject(pobj){
	}

signals:
    void startReplay(void);

public slots:
	/*
	void print(const QString& str)
	{
		qDebug() << str;
	}
	*/
    int alert(const QString& strMessage) {
		return QMessageBox::question(0, strMessage, u8"Вы уверены?");
	}
	
	void quitApplication() {
		qApp->quit();
	}
	/*
	QStringList dirEntryList(const QString& strDir, const QString& strExt)
	{
		QDir dir(strDir);
		return dir.entryList(strExt.split(" "), QDir::Files);
	}
	*/
};

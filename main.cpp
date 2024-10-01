#include <QJSEngine> 
#include <QObject>
#include "JSTools.h"
#include "pcscreen.h"
#include "rate.h"

#include "mainwindow.h"

//#define APP_LAUNCH_FROM_IDE

int main(int argc, char** argv){
    QApplication app(argc, argv);

    app.setOrganizationName("rffrb");
    app.setOrganizationDomain("rffrb.ru");
    app.setApplicationName("Tablo H2H");

    MainWindow* mWin = new MainWindow;

    PCScreen * pwgt = new PCScreen(mWin);
    pwgt->setObjectName("pwgt");

    mWin->setCentralWidget(pwgt);
    mWin->show();

    QObject::connect(mWin, SIGNAL(newFile()), pwgt, SLOT(newListSportsmens()));
    QObject::connect(mWin, SIGNAL(variant(int)), pwgt, SLOT(Variant(int)));

    QObject::connect(mWin, SIGNAL(sigLogo(bool)), pwgt, SIGNAL(sigLogo(bool)));

    //QObject::connect(mWin->ActStream, SIGNAL(toggled(bool)), pwgt, SLOT(turnTranslationToVk(bool)));

	QJSEngine se;

#ifdef APP_LAUNCH_FROM_IDE
    QFile  file("script.js");
    QFile jFile("data.json");
#else
    QFile  file("bin/script.js");
    QFile jFile("bin/data.json");
#endif

    QString val;
    jFile.open(QIODevice::ReadOnly | QIODevice::Text);
    val = jFile.readAll();
    jFile.close();

    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject jObj = doc.object();
    qDebug()<<"time = "<<jObj["blue_name"].toString()<<jObj["time"].toString()<<jObj["red_name"].toString()<<jObj["red_region"].toString()<<jObj["red_rate"].toString();
    qDebug()<<jObj;

    if (file.open(QFile::ReadOnly)) {

        QJSValue sw = se.newQObject((QObject *)pwgt);
		se.globalObject().setProperty("wgt", sw);

        QList<QObject*> lst = pwgt->findChildren<QObject*>();
        lst.prepend(pwgt);

		foreach(QObject* pobj, lst) {
			sw = se.newQObject(pobj);
			se.globalObject().setProperty(pobj->objectName(), sw);
		}
		JSTools* pjt = new JSTools;
		sw = se.newQObject(pjt);
		QString strClassName = pjt->metaObject()->className();
		se.globalObject().setProperty(strClassName, sw);
        QJSValue result = se.evaluate(QLatin1String(file.readAll()));
		if (result.isError()) {
			QMessageBox::critical(0,
								  "Evaluating error",
								  result.toString(),
								  QMessageBox::Yes);
		}
	}else {
		QMessageBox::critical(0,
							  "File open error",
                              " Can not open the script file",
							  QMessageBox::Yes);
	}

	return app.exec();
}


#include <QJSEngine> 
#include <QObject>
#include "JSTools.h"
#include "pcscreen.h"
#include "rate.h"

#include "mainwindow.h"


int main(int argc, char** argv){
    QApplication app(argc, argv);

    app.setOrganizationName("rffrb");
    app.setOrganizationDomain("rffrb.ru");
    app.setApplicationName("Tablo H2H");

    QLockFile lockFile(QDir::temp().absoluteFilePath("lurity.lock"));

    if (!lockFile.tryLock(100))
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Приложение уже запущено.\n"
                       "Разрешено запускать только один экземпляр приложения.");
        msgBox.exec();
        return 1;
    }



    MainWindow* mWin = new MainWindow;

    PCScreen * pwgt = new PCScreen(mWin);
    pwgt->setObjectName("pwgt");

    mWin->setCentralWidget(pwgt);
    mWin->show();

    QObject::connect(mWin, SIGNAL(newFile()), pwgt, SLOT(newListSportsmens()));
    QObject::connect(mWin, SIGNAL(variant(int)), pwgt, SLOT(Variant(int)));

    QObject::connect(mWin, SIGNAL(sigLogo(bool)), pwgt, SIGNAL(sigLogo(bool)));

    //QObject::connect(mWin, SIGNAL(sigExit(void)), pwgt, SLOT(slotExit(void)));

	QJSEngine se;

    QProcess proc;
    QList<QString> args;
    args<<"/c"<<"ffmpeg"<<"-codecs"<<"|"<<"findstr"<<"h264";
    proc.start("cmd", args);
    proc.waitForFinished();
    qDebug()<<proc.readAllStandardOutput();


    QFile  file("script.js");
    QFile jFile("data.json");


    QString val;
    jFile.open(QIODevice::ReadOnly | QIODevice::Text);
    val = jFile.readAll();
    jFile.close();

    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject jObj = doc.object();
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


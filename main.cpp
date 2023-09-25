#include <QJSEngine> 
#include <QObject>
#include "JSTools.h"
#include "pcscreen.h"
#include "rate.h"

#include "mainwindow.h"

//#define APP_LAUNCH_FROM_IDE

int main(int argc, char** argv){
    QApplication app(argc, argv);

    //QCoreApplication::setOrganizationName("zak2791");
    //QCoreApplication::setApplicationName("Tablo H2H");

    MainWindow* mWin = new MainWindow;

    PCScreen * pwgt = new PCScreen(mWin);
    pwgt->setObjectName("pwgt");

    mWin->setCentralWidget(pwgt);
    mWin->show();

    QObject::connect(mWin, SIGNAL(newFile()), pwgt, SLOT(newListSportsmens()));
    QObject::connect(mWin, SIGNAL(variant(int)), pwgt, SLOT(Variant(int)));

	QJSEngine se;

#ifdef APP_LAUNCH_FROM_IDE
    QFile  file("script.js");
#else
    QFile  file("bin/script.js");
#endif
	


    if (file.open(QFile::ReadOnly)) {

        QJSValue sw = se.newQObject((QObject *)pwgt);
		se.globalObject().setProperty("wgt", sw);

        QList<QObject*> lst = pwgt->findChildren<QObject*>();
        lst.prepend(pwgt);

		foreach(QObject* pobj, lst) {
			sw = se.newQObject(pobj);
			se.globalObject().setProperty(pobj->objectName(), sw);
            //qDebug()<<pobj->objectName();
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
							  "Can not open the script file",
							  QMessageBox::Yes);
	}
    //qDebug()<<333;
	return app.exec();
}


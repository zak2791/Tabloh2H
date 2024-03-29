#include <QJSEngine> 
#include <QObject>
#include "JSTools.h"
#include "pcscreen.h"
#include "rate.h"




int main(int argc, char** argv){
    QApplication app(argc, argv);

    //QCoreApplication::setOrganizationName("zak2791");
    //QCoreApplication::setApplicationName("Tablo H2H");


    PCScreen * pwgt = new PCScreen();
    pwgt->setObjectName("pwgt");

	QJSEngine se;
	
    QFile  file("script.js");
	
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


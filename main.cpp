#include <QJSEngine> 
#include <QObject>
#include "JSTools.h"
#include "pcscreen.h"
#include "rate.h"
#include "filter.h"



int main(int argc, char** argv){
    QApplication app(argc, argv);

	//Ball * b = new Ball(nullptr, 99, false);
    //b->show();
	
    PCScreen * pwgt = new PCScreen();
	
    //KeyFilter kf(pwgt);
    //app.installEventFilter(&kf);
    //pwgt->installEventFilter(&kf);

	QJSEngine se;
	
    QFile  file("script.js");
	
    if (file.open(QFile::ReadOnly)) {

        QJSValue sw = se.newQObject((QObject *)pwgt);
		se.globalObject().setProperty("wgt", sw);

        QList<QObject*> lst = pwgt->findChildren<QObject*>();
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
    qDebug()<<333;
	return app.exec();
}


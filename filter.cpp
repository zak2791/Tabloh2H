#include "filter.h"

KeyFilter::KeyFilter(QObject* pobj) : QObject(pobj)
{
}

bool KeyFilter::eventFilter(QObject * , QEvent * pe) {
	//qDebug() << "pe->type()" << pe->type();
	if (pe->type() == QEvent::KeyPress) {
		QKeyEvent * ke = static_cast<QKeyEvent*>(pe);
        if (ke->key() == Qt::Key_Up){
            //emit sig_key(0);
			qDebug() << "up";
            return true;
        }else if (ke->key() == Qt::Key_Down){
            //emit sig_key(1);
			qDebug() << "down";
            return true;
        }else if (ke->key() == Qt::Key_Right){
            //emit sig_key(2);
			qDebug() << "right";
            return true;
        }else if (ke->key() == Qt::Key_Left){
            //emit sig_key();
			qDebug() << "left";
            return true;
        }
        return false;
	}
	return false;	// QObject::eventFilter(pobj, pe);
}

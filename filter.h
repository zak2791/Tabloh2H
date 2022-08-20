#pragma once

#include <QObject> 
#include <QEvent>
#include <QKeyEvent>
#include <QDebug> 

class KeyFilter : public QObject {
protected:
	virtual bool eventFilter(QObject *, QEvent *);
public:
	KeyFilter(QObject * pobj = 0);
signals:
	void sig_key();
};

#ifndef RECLABEL_H
#define RECLABEL_H

#include "qlabel.h"
#include "qsvgrenderer.h"

class RecLabel : public QLabel
{
    Q_OBJECT
public:
    explicit RecLabel(QWidget *parent = nullptr);

    void setStatusRec(bool);

private:
    virtual void paintEvent(QPaintEvent* ev);
    virtual void resizeEvent(QResizeEvent* ev);
    const QString path_no_active = ":/images/rec_no_active.svg";
    const QString path_blink = ":/images/rec_blink.svg";
    const QString path = ":/images/rec.svg";
    QSvgRenderer* rendererRec;
    QSvgRenderer* rendererRecBlink;
    QSvgRenderer* rendererRecNoActive;
    QSvgRenderer* rendererRecCurrent;

signals:
};

#endif // RECLABEL_H

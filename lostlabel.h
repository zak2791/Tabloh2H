#ifndef LOSTLABEL_H
#define LOSTLABEL_H

#include "qlabel.h"
#include "qsvgrenderer.h"

class LostLabel : public QLabel
{
public:
    LostLabel(QWidget *parent = nullptr);

    void setLostFrames(int);

private:
    virtual void paintEvent(QPaintEvent* ev);
    virtual void resizeEvent(QResizeEvent* ev);
    const QString path_active = ":/images/lost_active.svg";
    const QString path_no_active = ":/images/lost_no_active.svg";
    QSvgRenderer* rendererLostActive;
    QSvgRenderer* rendererLostNoActive;

    int lostFrames;


};

#endif // LOSTLABEL_H

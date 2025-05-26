#ifndef VKLABEL_H
#define VKLABEL_H

#include "qlabel.h"
#include "qsvgrenderer.h"

class VkLabel : public QLabel
{
    Q_OBJECT
public:
    explicit VkLabel(QWidget *parent = nullptr);

    void setStatusVk(bool);
    bool getStatusVk(void){return status;}

private:
    virtual void paintEvent(QPaintEvent* ev);
    virtual void resizeEvent(QResizeEvent* ev);
    const QString path_no_active = ":/images/vk.svg";
    const QString path_blink = ":/images/vk_gray_blue.svg";
    const QString path = ":/images/vk_blue.svg";
    QSvgRenderer* rendererVk;
    QSvgRenderer* rendererVkBlink;
    QSvgRenderer* rendererVkNoActive;
    QSvgRenderer* rendererVkCurrent;
    bool status;

signals:
};

#endif // VKLABEL_H

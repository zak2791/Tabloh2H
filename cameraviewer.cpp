#include "cameraviewer.h"
#include <QImage>
#include <QDebug>


CameraViewer::CameraViewer(QWidget* parent) : QGraphicsView(parent){
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scene = new QGraphicsScene(this);
    pi = new QGraphicsPixmapItem();
    p = parent;
}

void CameraViewer::draw_image(QImage img){
    if(pi)
        delete pi;
    pi = scene->addPixmap(QPixmap::fromImage(img.scaled(p->width(), p->height())));
    scene->setSceneRect(this->rect());
    //qDebug()<<p->rect();
    setScene(scene);
}

void CameraViewer::setSize(int w, int h){
    //scene->setSceneRect(0, 0, w, h);
    //setScene(scene);
}

void CameraViewer::resizeEvent(QResizeEvent* e){
    //qDebug()<<"resize"<<e->size();
    //scene->setSceneRect(0, 0, e->size().width(), e->size().height());
    //setScene(scene);
}

  #include "cameraviewer.h"
#include <QImage>
#include <QDebug>


CameraViewer::CameraViewer(QWidget* parent) : QGraphicsView(parent){
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scene = new QGraphicsScene(this);
    pi = new QGraphicsPixmapItem();
}

void CameraViewer::draw_image(QImage img){
    if(pi)
        delete pi;
    pi = scene->addPixmap(QPixmap::fromImage(img.scaled(this->width(), this->height())));
    scene->setSceneRect(this->rect());
    setScene(scene);
}

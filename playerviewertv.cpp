#include "playerviewertv.h"
#include <QDebug>

PlayerViewerTV::PlayerViewerTV(){
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scene = new QGraphicsScene(this);
    pi = new QGraphicsPixmapItem();
}

void PlayerViewerTV::draw_image(QImage img){
    if(pi)
        delete pi;
    pi = scene->addPixmap(QPixmap::fromImage(img.scaled(this->width(), this->height())));

    scene->setSceneRect(this->rect());

    setScene(scene);
}

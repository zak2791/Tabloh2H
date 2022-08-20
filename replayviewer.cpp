#include "replayviewer.h"
#include <QDebug>

ReplayViewer::ReplayViewer(QWidget* parent) : QGraphicsView(parent)
{
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scene = new QGraphicsScene(this);

    setStyleSheet("background-color: transparent");
    //setFrameStyle(QFrame::Box);
    //setFrameStyle("")
    //raise();
    camera = 0;
    tmr = new QTimer();
    connect(tmr, SIGNAL(timeout()), SLOT(set_image()));
    tmr->start(60);
    this->setMinimumHeight(100);
    this->setMinimumWidth(100);
    p = this;
}

void ReplayViewer::set_image(){
    static int c = 0;
    if(camera == 1){
        if(!que1.isEmpty()){
            QImage image = que1.dequeue().scaled(this->width(), this->height());
            if(scene->items().count() > 0)
                delete pi;
            //pi = scene->addPixmap(QPixmap::fromImage(image));
            QPixmap pix = QPixmap::fromImage(image.scaled(p->width(), p->height()));
            //QPixmap pix2 = pix.copy(pix.width()/4,pix.height()/4,pix.width()/2,pix.height()/2);
            pi = scene->addPixmap(pix.scaled(p->width(), p->height()));
            //scene->setSceneRect(this->width()/(-4),this->height()/(-4),this->width()*2, this->height()*2);
            scene->setSceneRect(p->rect());
            //qDebug()<<this->rect();
            this->setScene(scene);
        }else if(!que2.isEmpty()){
            QImage image = que2.dequeue().scaled(this->width(), this->height());
            if(scene->items().count() > 0)
                delete pi;
            pi = scene->addPixmap(QPixmap::fromImage(image));
            //QPixmap pix = QPixmap::fromImage(image);
            //QPixmap pix2 = pix.copy(pix.width()/4,pix.height()/4,pix.width()/2,pix.height()/2);
            //pi = scene->addPixmap(pix2);
            scene->setSceneRect(p->rect());
            this->setScene(scene);
        }else{
            QImage alpha(this->width(), this->height(), QImage::Format_Alpha8);
            alpha.fill(0);
            if(scene->items().count() > 0)
                delete pi;
            pi = scene->addPixmap(QPixmap::fromImage(alpha));
            //scene->setSceneRect(this->width()/(-4),this->height()/(-4),this->width()*2, this->height()*2);

            scene->setSceneRect(this->rect());
            this->setScene(scene);
            c = 0;
            camera = 0;
            emit end_replay();
        }
    }else if(camera == 2){
        if(!que2.isEmpty()){
            QImage image = que2.dequeue().scaled(this->width(), this->height());

            if(scene->items().count() > 0)
                delete pi;

            //QPixmap pix = QPixmap::fromImage(image);
            //QPixmap pix2 = pix.copy(pix.width()/4,pix.height()/4,pix.width()/2,pix.height()/2);
            //pi = scene->addPixmap(pix2);
            pi = scene->addPixmap(QPixmap::fromImage(image));
            scene->setSceneRect(this->rect());
            //scene->setSceneRect(this->width()/(-4),this->height()/(-4),this->width()*2, this->height()*2);
            this->setScene(scene);
        }else if(!que1.isEmpty()){
            QImage image = que1.dequeue().scaled(this->width(), this->height());
            if(scene->items().count() > 0)
                delete pi;
            pi = scene->addPixmap(QPixmap::fromImage(image));
            scene->setSceneRect(this->rect());
            this->setScene(scene);
        }else{
            QImage alpha(this->width(), this->height(), QImage::Format_Alpha8);
            alpha.fill(0);
            if(scene->items().count() > 0)
                delete pi;
            pi = scene->addPixmap(QPixmap::fromImage(alpha));
            //scene->setSceneRect(this->width()/(-4),this->height()/(-4),this->width()*2, this->height()*2);
            scene->setSceneRect(this->rect());
            this->setScene(scene);
            c = 0;
            camera = 0;
            emit end_replay();
        }
    }
}



void ReplayViewer::draw_image(QImage img, int cam){
    if(camera == 0){
        camera = cam;
    }
    if(cam == 1)
        que1.enqueue(img);
    if(cam == 2)
        que2.enqueue(img);
}

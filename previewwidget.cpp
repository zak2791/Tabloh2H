#include "previewwidget.h"
#include "qevent.h"
#include "qgraphicsitem.h"

#include <qboxlayout.h>
#include <QGraphicsVideoItem>

PreviewWidget::PreviewWidget(QWidget *parent) : QGraphicsView(parent) {

    player = new QMediaPlayer(this);

    scene = new QGraphicsScene();

    QGraphicsVideoItem * item = new QGraphicsVideoItem;
    item->setAspectRatioMode(Qt::IgnoreAspectRatio);

    player->setVideoOutput(item);

    scene->addItem(item);
    //item->setPos(0, 0);

    //item->setSize(QSize(scene->width(), scene->height()));




    setScene(scene);


}

void PreviewWidget::play(){
    player->play();
}

void PreviewWidget::setSource(QString url){
    player->setSource(QUrl(url));
}

void PreviewWidget::resizeEvent(QResizeEvent * e)
{
    scene->setSceneRect(0, 0, e->size().width(), e->size().height());
}

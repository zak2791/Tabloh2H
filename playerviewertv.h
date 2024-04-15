#ifndef PLAYERVIEWERTV_H
#define PLAYERVIEWERTV_H

#include <QObject>

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

class PlayerViewerTV : public QGraphicsView
{
    Q_OBJECT

public:
    PlayerViewerTV();

private:
    QGraphicsScene *scene;
    QGraphicsPixmapItem *pi;

public slots:
    void draw_image(QImage);

};




#endif // PLAYERVIEWERTV_H

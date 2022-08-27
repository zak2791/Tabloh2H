#ifndef CAMERAVIEWER_H
#define CAMERAVIEWER_H

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QResizeEvent>

class CameraViewer : public QGraphicsView
{
    Q_OBJECT
public:
    CameraViewer(QWidget* parent=nullptr);

private:
    QGraphicsScene* scene;
    QGraphicsPixmapItem* pi;

public slots:
    void draw_image(QImage);

};

#endif // CAMERAVIEWER_H

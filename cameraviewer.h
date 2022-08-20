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

    virtual void resizeEvent(QResizeEvent*);

private:
    QGraphicsScene* scene;
    QGraphicsPixmapItem* pi;
    QWidget* p;

public slots:
    void draw_image(QImage);
    void setSize(int, int);

};

#endif // CAMERAVIEWER_H

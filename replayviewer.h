#ifndef REPLAYVIEWER_H
#define REPLAYVIEWER_H

#include <QTimer>
#include <QQueue>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>

class ReplayViewer : public QGraphicsView
{
    Q_OBJECT

public:
    ReplayViewer(QWidget* parent=nullptr);

private:
    QTimer* tmr;
    QQueue<QImage> que1;
    QQueue<QImage> que2;
    QGraphicsScene* scene;
    QGraphicsPixmapItem* pi;
    int camera;
    QWidget* p;

signals:
    void end_replay(void);

public slots:
    void draw_image(QImage, int);

private slots:
    void set_image(void);

};

#endif // REPLAYVIEWER_H

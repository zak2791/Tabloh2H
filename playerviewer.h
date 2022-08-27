#ifndef PLAYERVIEWER_H
#define PLAYERVIEWER_H

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QWidget>
#include <QSlider>
#include <QPushButton>
#include "player.h"
#include <QThread>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPaintEvent>

class PlayerViewer : public QGraphicsView
{
    Q_OBJECT

public:
    PlayerViewer(QString file = "", QWidget* parent=nullptr);

private:
    QGraphicsScene *scene;
    QGraphicsPixmapItem *pi;
    QSlider *slider;
    QHBoxLayout *layout;
    QFrame* sizeBufferFrames;
    QFrame* markerCurrentFrame;
    QPushButton* btnPlay;
    QPushButton* btnNextFrame;
    QPushButton* btnPreviewFrame;
    QPushButton* btnClose;

    Player* player;
    QThread* thread;

    virtual void resizeEvent(QResizeEvent*);
    virtual void closeEvent(QCloseEvent*);
    virtual void keyPressEvent(QKeyEvent*);

    int maxLenghtBuffer;
    QString videoFile;

private slots:
    void turnPlay(void);
    void nextFrame(void);
    void previewFrame(void);
    void setSeek(int);
    void draw_image(QImage);
    void parametersMedia(int, int, int);
    void drawBuffer(int, int);

signals:
    void sigClose(void);

};



#endif // PLAYERVIEWER_H

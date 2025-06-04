#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include "qmediaplayer.h"
#include "qwidget.h"

#include <qgraphicsview.h>

class PreviewWidget : public QGraphicsView
{
public:
    PreviewWidget(QWidget *parent = nullptr);
    void play(void);
    void setSource(QString);
    void resizeEvent(QResizeEvent*);

private:
    QMediaPlayer* player;
    QGraphicsScene *scene;

};

#endif // PREVIEWWIDGET_H

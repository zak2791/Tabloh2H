#ifndef PLAYERTV_H
#define PLAYERTV_H

#include "qmediaplayer.h"
#include "qwidget.h"

class PlayerTv : public QWidget
{
public:
    PlayerTv(QWidget *parent = nullptr);
    QMediaPlayer* player;
};

#endif // PLAYERTV_H

#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>

class Player : public QObject
{
    Q_OBJECT


public:
    explicit Player(QString file = "", QObject *parent = nullptr);
    ~Player();

    static QStringList getListWebCams(void);
    static QList<QList<int> > getListParamWebCam(QString);
    static QStringList getListSoundDevices(void);

};

#endif // PLAYER_H

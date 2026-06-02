#ifndef MAIN_H
#define MAIN_H

#include "qstringview.h"
#include "qtypes.h"
struct packet{
    //int durPacket;
    //long durStream;
    qlonglong pts;
    QByteArray data;
    int flags;
};

#endif // MAIN_H


QT += core gui widgets qml
HEADERS += \
    Ball.h \
    First_display.h \
    JSTools.h \
    Second_display.h \
    Secundomer.h \
    cameraviewer.h \
    category.h \
    fam_reg.h \
    filter.h \
    list_family.h \
    narusheniya_pravil.h \
    narusheniya_vyhod.h \
    plus.h

SOURCES += \
    Ball.cpp \
    First_display.cpp \
    JSTools.cpp \
    Second_display.cpp \
    Secundomer.cpp \
    cameraviewer.cpp \
    category.cpp \
    fam_reg.cpp \
    filter.cpp \
    list_family.cpp \
    main.cpp \
    narusheniya_pravil.cpp \
    narusheniya_vyhod.cpp \
    plus.cpp

FORMS += \
    FormView.ui \
    frmTime.ui

DISTFILES += \
    ../build-TabloRB-Desktop_Qt_5_14_2_MinGW_64_bit-Debug/script.js \
    ../build-TabloRB-Desktop_Qt_5_14_2_MinGW_64_bit-Debug/script.js

win32: LIBS += -L$$PWD/ffmpeg/lib/ -lavformat

INCLUDEPATH += $$PWD/ffmpeg/include
DEPENDPATH += $$PWD/ffmpeg/include

win32: LIBS += -L$$PWD/ffmpeg/lib/ -lavutil

win32: LIBS += -L$$PWD/ffmpeg/lib/ -lavcodec

win32: LIBS += -L$$PWD/ffmpeg/lib/ -lavdevice

win32: LIBS += -L$$PWD/ffmpeg/lib/ -lavfilter

win32: LIBS += -L$$PWD/ffmpeg/lib/ -lswscale

win32: LIBS += -L$$PWD/ffmpeg/lib/ -lswresample

# QXlsx code for Application Qt project
QXLSX_PARENTPATH=./         # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=./header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=./source/  # current QXlsx source path is ./source/
include(./QXlsx.pri)
#CONFIG     += debug

QMAKE_CXXFLAGS_RELEASE -= -O3
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE -= -O0

RC_ICONS = appico.ico

QT += core gui widgets qml multimedia svg quickwidgets
HEADERS += \
    JSTools.h \
    addform.h \
    camera.h \
    cameraconnection.h \
    cameraviewer.h \
    category.h \
    choosingnames.h \
    endtime.h \
    fam_reg.h \
    firstlastname.h \
    helpform.h \
    lcdstopwatch.h \
    lcdtimer.h \
    list_family.h \
    listview.h \
    logofrb.h \
    mainlistmodel.h \
    mainwindow.h \
    narusheniya_pravil.h \
    newsportsman.h \
    pcscreen.h \
    player.h \
    playerviewer.h \
    playerviewertv.h \
    plus.h \
    rate.h \
    sidelistmodel.h \
    sportsman.h \
    togglebutton.h \
    tvscreen.h

SOURCES += \
    JSTools.cpp \
    addform.cpp \
    camera.cpp \
    cameraconnection.cpp \
    cameraviewer.cpp \
    category.cpp \
    choosingnames.cpp \
    endtime.cpp \
    fam_reg.cpp \
    firstlastname.cpp \
    helpform.cpp \
    lcdstopwatch.cpp \
    lcdtimer.cpp \
    list_family.cpp \
    logofrb.cpp \
    main.cpp \
    mainlistmodel.cpp \
    mainwindow.cpp \
    narusheniya_pravil.cpp \
    newsportsman.cpp \
    pcscreen.cpp \
    player.cpp \
    playerviewer.cpp \
    playerviewertv.cpp \
    plus.cpp \
    rate.cpp \
    sidelistmodel.cpp \
    sportsman.cpp \
    togglebutton.cpp \
    tvscreen.cpp

FORMS += \
    FormView.ui \
    addform.ui \
    frmTime.ui \
    frmTv.ui \
    helpform.ui \
    mainwindow.ui \
    new_sportsman.ui \
    version.ui

DISTFILES += \
    ../build-TabloRB-Desktop_Qt_5_14_2_MinGW_64_bit-Debug/script.js \
    ../build-TabloRB-Desktop_Qt_5_14_2_MinGW_64_bit-Debug/script.js \
    script.js

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

#CONFIG+=qml_debug

RESOURCES += \
    res.qrc

DEFINES += APP_LAUNCH_FROM_IDE


QMAKE_CXXFLAGS_RELEASE -= -O3
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE -= -O0

RC_ICONS = appico.ico

QT += core \
       gui\
        widgets \
       qml \
       multimedia \
       svg \
       quickwidgets \
       network \
       multimediawidgets \
       svgwidgets

HEADERS += \
    JSTools.h \
    addform.h \
    cameraconnection.h \
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
    playerpc.h \
    playertv.h \
    playerviewertv.h \
    plus.h \
    rate.h \
    reclabel.h \
    settingsvideoreplay.h \
    sidelistmodel.h \
    sportsman.h \
    svgbutton.h \
    tvscreen.h \
    videoreplaycontrol.h \
    vklabel.h

  SOURCES += \
    JSTools.cpp \
    addform.cpp \
    cameraconnection.cpp \
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
    playerpc.cpp \
    playertv.cpp \
    playerviewertv.cpp \
    plus.cpp \
    rate.cpp \
    reclabel.cpp \
    settingsvideoreplay.cpp \
    sidelistmodel.cpp \
    sportsman.cpp \
    tvscreen.cpp \
    videoreplaycontrol.cpp \
    vklabel.cpp

FORMS += \
    FormSettingsVideoReplay.ui \
    FormView.ui \
    addform.ui \
    code.ui \
    frmTime.ui \
    helpform.ui \
    mainwindow.ui \
    new_sportsman.ui \
    settingsvideoreplay.ui \
    version.ui \
    videoreplaycontrol.ui

DISTFILES += \
    script.js \
    sound/gong.wav

# win32: LIBS += -L$$PWD/ffmpeg/lib/ -lavformat

# INCLUDEPATH += $$PWD/ffmpeg/include
# DEPENDPATH += $$PWD/ffmpeg/include

# win32: LIBS += -L$$PWD/ffmpeg/lib/ -lavutil

# win32: LIBS += -L$$PWD/ffmpeg/lib/ -lavcodec

# win32: LIBS += -L$$PWD/ffmpeg/lib/ -lavdevice

# win32: LIBS += -L$$PWD/ffmpeg/lib/ -lavfilter

# win32: LIBS += -L$$PWD/ffmpeg/lib/ -lswscale

# win32: LIBS += -L$$PWD/ffmpeg/lib/ -lswresample

# #QXlsx code for Application Qt project
QXLSX_PARENTPATH=./         # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=./header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=./source/  # current QXlsx source path is ./source/
include(./QXlsx.pri)
#CONFIG     += debug

#CONFIG+=qml_debug

RESOURCES  += \
    res.qrc

#DEFINES += APP_LAUNCH_FROM_IDE

#DEFINE += QT_DISABLE_DEPRECATED_UP_TO=0x050F00


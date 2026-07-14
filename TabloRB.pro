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
       svgwidgets \
       httpserver \
       concurrent \
       openglwidgets

HEADERS += $$files(*.h) \
           $$files(UsbCamera/*.h) \
           $$files(ffmpeg/**/*.h)

SOURCES += $$files(*.cpp) \
           $$files(UsbCamera/*.cpp)

FORMS += \
    CameraController.ui \
    FormSettingsVideoReplay.ui \
    FormView.ui \
    addform.ui \
    code.ui \
    frmTime.ui \
    helpform.ui \
    mainwindow.ui \
    new_sportsman.ui \
    settingsvideoreplay.ui \
    settingswebcamera.ui \
    version.ui \
    videoreplaycontrol.ui \
    videoreplaycontrolusb.ui

DISTFILES += $$files(ffmpeg/lib/*.def) \
             $$files(ffmpeg/lib/*.dll) \
             $$files(ffmpeg/lib/*.lib) \
             $$files(docs/*, true) \
             script.js

win32: LIBS += -L$$PWD/ffmpeg/lib/ -lavformat

INCLUDEPATH += $$PWD/ffmpeg/include UsbCamera
DEPENDPATH += $$PWD/ffmpeg/include

win32: LIBS += -L$$PWD/ffmpeg/lib/ -lavutil

win32: LIBS += -L$$PWD/ffmpeg/lib/ -lavcodec

win32: LIBS += -L$$PWD/ffmpeg/lib/ -lavdevice

win32: LIBS += -L$$PWD/ffmpeg/lib/ -lavfilter

win32: LIBS += -L$$PWD/ffmpeg/lib/ -lswscale

win32: LIBS += -L$$PWD/ffmpeg/lib/ -lswresample

defineTest(copyToDestDir) {
    files = $$1
    dir = $$2
    win32:dir ~= s,/,\\,g

    for(file, files) {

        win32:file ~= s,/,\\,g

        QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$shell_quote($$file) $$shell_quote($$dir) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}

copyToDestDir($$PWD/script.js, $$OUT_PWD/)
copyToDestDir($$PWD/docs/, $$OUT_PWD/)

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


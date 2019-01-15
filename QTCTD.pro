#-------------------------------------------------
#
# Project created by QtCreator 2018-12-19T15:53:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QTCTD
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    templateprepare.cpp \
    function.cpp

HEADERS += \
        mainwindow.h \
    templateprepare.h \
    function.h

FORMS += \
        mainwindow.ui

win32{
message("compile for windows")

QMAKE_CXXFLAGS += /MP

# opencv
INCLUDEPATH+= D:\OpenCV\vs2015-64build-opencv3.3\install\include    \
                D:\Python\Lib\site-packages\numpy\core\include  \
                D:\Python\include

CONFIG(debug, debug|release) {
message("debug")
#LIBS  +=  D:\OpenCV\vs2017-64build\install\lib\opencv_*2410d.lib
LIBS += D:\OpenCV\vs2015-64build-opencv3.3\install\x64\vc14\lib\opencv_*330d.lib
LIBS += D:\Python\libs\python36_d.lib   \
        D:\Python\libs\python3_d.lib    \
        D:\Python\libs\_tkinter.lib
} else {
message("release")
#LIBS  +=  D:\OpenCV\vs2017-64build\install\lib\opencv_*2410.lib
LIBS += D:\OpenCV\vs2015-64build-opencv3.3\install\x64\vc14\lib\opencv_*330.lib
LIBS += D:\Python\libs\python36.lib \
        D:\Python\libs\python3.lib
}



#DAHENG
INCLUDEPATH += $$quote(D:\Program Files\Daheng Imavision\MER-Series\Samples\VC SDK\inc)
LIBS += $$quote(D:\Program Files\Daheng Imavision\MER-Series\Samples\VC SDK\lib\x64\*.lib)
}
linux{
    QMAKE_DEFAULT_INCDIRS = ...

    INCLUDEPATH += /usr/local/include \
        /usr/include/opencv \
        /usr/include/opencv2

    INCLUDEPATH+=/usr/include   \
        /usr/include/python3.6m \
        /home/qian/.local/lib/python3.6/site-packages/numpy/core/include/


    if(contains(DEFINES,ARM)){
        message("compile for arm linux")
        LIBS += /usr/lib/aarch64-linux-gnu/libmysqlclient.so
        LIBS += /usr/lib/libopencv_*.so.3.3
    }else{
        message("compile for amd linux")
        LIBS += /usr/lib/x86_64-linux-gnu/libmysqlclient.so
        LIBS += /usr/lib/x86_64-linux-gnu/libopencv_*.so.3.2
        LIBS += /usr/lib/python3.6/config-3.6m-x86_64-linux-gnu/libpython3.6.so
    }
}

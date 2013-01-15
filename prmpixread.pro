#-------------------------------------------------
#
# Project created by QtCreator 2012-02-22T19:57:44
#
#-------------------------------------------------

QT       += core gui
#CONFIG += console

#INCLUDEPATH += c:/dev/qwt-6.0.1/src
#LIBS += "c:/dev/qwt-6.0.1/lib/libqwtd.a"
#INCLUDEPATH += c:/opt/qwt-6.0.1/src
#LIBS += "c:/opt/qwt-6.0.1/lib/libqwtd.a"

include( qwtconfig.pri )

QWT_ROOT = "c:/dev/qwt-6.0.1"
#QWT_ROOT = "c:/opt/qwt-6.0.1"
INCLUDEPATH += $${QWT_ROOT}/src

TARGET = prmpixread
TEMPLATE = app

#if(!debug_and_release|build_pass):CONFIG(debug, debug|release) {
# mac:LIBS = $$member(LIBS, 0) \
# $$member(LIBS, 1)_debug
#    win32:LIBS = $$member(LIBS,0)\
# $$member(LIBS, 1)d
#}

QMAKE_RPATHDIR *= $${QWT_ROOT}/lib

contains(QWT_CONFIG, QwtFramework) {

    LIBS      += -F$${QWT_ROOT}/lib
}
else {

    LIBS      += -L$${QWT_ROOT}/lib
}

IPATH       = $${INCLUDEPATH}
qtAddLibrary(qwt)
INCLUDEPATH = $${IPATH}

contains(QWT_CONFIG, QwtSvg) {

    QT += svg
}
else {

    DEFINES += QWT_NO_SVG
}


win32 {
    contains(QWT_CONFIG, QwtDll) {
        DEFINES    += QT_DLL QWT_DLL
    }
}

SOURCES += main.cpp prmpixread.cpp matrixofpixels.cpp \
    histogram.cpp
HEADERS  += prmpixread.h matrixofpixels.h \
    histogram.h \
    version.h
FORMS    += prmpixread.ui
win32:RC_FILE = rpmp.rc








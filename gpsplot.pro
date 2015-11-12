#-------------------------------------------------
#
# Project created by QtCreator 2014-09-29T15:51:12
#
#-------------------------------------------------

QT       += core gui xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gpsplot
TEMPLATE = app
INCLUDEPATH += hdr \
    /usr/include/qwt6

SOURCES  += src/main.cpp\
    src/gpsplot.cpp \
    src/latlon.cpp \
    src/gpgraph.cpp \
    src/plotdata.cpp \
    src/filedownloader.cpp \
    src/gplatlon.cpp \
    src/gpmapplot.cpp \
    src/gploading.cpp

HEADERS  += hdr/gpsplot.h \
    hdr/gpgraph.h \
    hdr/plotdata.h \
    hdr/latlon.h \
    hdr/filedownloader.h \
    hdr/gplatlon.h \
    hdr/gpmapplot.h \
    hdr/gploading.h

FORMS    += frm/gpsplot.ui \
    frm/gpgraph.ui \
    frm/gplatlon.ui \
    frm/gploading.ui

LIBS     += -L/usr/lib64 -lqwt

OTHER_FILES += \
    README

RESOURCES += \
    gpsplot.qrc

DISTFILES +=

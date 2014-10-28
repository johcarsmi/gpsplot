#-------------------------------------------------
#
# Project created by QtCreator 2014-09-29T15:51:12
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gpsplot
TEMPLATE = app
INCLUDEPATH += hdr \
    /usr/include/qwt6

SOURCES  += src/main.cpp\
    src/gpsplot.cpp \
    src/latlon.cpp \
    src/gpgraph.cpp \
    src/plotdata.cpp

HEADERS  += hdr/gpsplot.h \
    hdr/gpgraph.h \
    hdr/plotdata.h

FORMS    += frm/gpsplot.ui \
    frm/gpgraph.ui

LIBS     += -L/usr/lib64 -lqwt

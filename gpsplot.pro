#-------------------------------------------------
#
# Project created by QtCreator 2014-09-29T15:51:12
#
#-------------------------------------------------

QT       += core gui xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

target.path = /usr/local/bin
INSTALLS += target

icon.path = /usr/share/icons/hicolor/48x48/apps     # freedesktop.org standard location for icons
icon.files = misc/gpsplot.png
INSTALLS += icon

menu.path = /usr/share/applications                 # freedesktop.org standard location for menus
menu.files = misc/gpsplot.desktop
INSTALLS += menu

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
    README \
    gpsplot.png \
    gpsplot.desktop

RESOURCES += \
    gpsplot.qrc

DISTFILES += \
    misc/gpsplot.desktop

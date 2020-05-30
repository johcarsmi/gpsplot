#-------------------------------------------------
#
# Project created by QtCreator 2014-09-29T15:51:12
#
#-------------------------------------------------

QT       += core widgets gui xml network
CONFIG   += console # not sure this is necessary - inserted to try to get qDebug() << ... to work

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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
    /usr/local/qwt-6.1.3/include

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

#LIBS     += -L/usr/lib64 -lqwt #Replaced this line with the one below in case
#                               #it made a difference.
LIBS        += -L/usr/local/qwt-6.1.3/lib/ -lqwt

OTHER_FILES += \
    README \
    gpsplot.png \
    gpsplot.desktop

RESOURCES += \
    gpsplot.qrc

DISTFILES += \
    misc/gpsplot.desktop \
    readme.md

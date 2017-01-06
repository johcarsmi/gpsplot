#include "gpsplot.h"
#include <qt5/QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GpsPlot w;
    w.show();
    return a.exec();
}

#include "gpsplot.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GpsPlot w;
    w.show();

    return a.exec();
}

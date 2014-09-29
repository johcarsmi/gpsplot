#ifndef GPSPLOT_H
#define GPSPLOT_H

#include <QMainWindow>

namespace Ui {
class GpsPlot;
}

class GpsPlot : public QMainWindow
{
    Q_OBJECT

public:
    explicit GpsPlot(QWidget *parent = 0);
    ~GpsPlot();

private:
    Ui::GpsPlot *ui;
};

#endif // GPSPLOT_H

#include "gpsplot.h"
#include "ui_gpsplot.h"

GpsPlot::GpsPlot(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GpsPlot)
{
    ui->setupUi(this);
}

GpsPlot::~GpsPlot()
{
    delete ui;
}

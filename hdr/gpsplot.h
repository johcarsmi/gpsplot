#ifndef GPSPLOT_H
#define GPSPLOT_H

#include <QMainWindow>
#include <QVector>
#include <QXmlStreamReader>

#include "hdr/plotdata.h"
#include "hdr/gpgraph.h"

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
    QString fileName;
    QString eleText;
    QXmlStreamReader xRead;
    bool dok;
    QVector <double> lat;   // Latitude
    QVector <double> lon;   // Longitude
    QVector <double> ele;   // Elevation
    QVector <double> tim;   // Time
    QVector <double> dst;   // Distance
    QVector <double> hspd;  // Horizontal Speed
    QVector <double> vspd;  // Vertical Speed
    bool firsttime;
    QString trkDate;

    void processFile(const QString &);
    void process_trk(QXmlStreamReader &);
    void process_trkseg(QXmlStreamReader &);
    void createGraph();
    void calcDst();
    void calcSpeed();
    void calcSpeed2();
    PlotData *pData;
    GpGraph *ggForm;

private slots:
    void doClose();
    void doOpen();
    void doPlot();
};

#endif // GPSPLOT_H

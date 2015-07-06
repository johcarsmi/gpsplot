#ifndef GPSPLOT_H
#define GPSPLOT_H

#include <QDateTime>
#include <QFile>
#include <QMainWindow>
#include <QTextStream>
#include <QVector>
#include <QXmlStreamReader>
#include <QSettings>

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
    QVector <double> tim2;  // Data at chosen time interval
    double latMax;
    double latMin;
    double lonMax;
    double lonMin;
    double xCtr;
    double yCtr;
    bool firsttime;
    QString trkDate;
    QFile oFile;
    QTextStream tStrm;
    QDateTime stTime;
    int elapsedTime;
    PlotData *pData;
    GpGraph *ggForm;
    QSettings * settings;
    const QString stgsFile = ".gpsplot.settings";
    QString curDir;

    void processFile(const QString &);
    void process_trk(QXmlStreamReader &);
    void process_trkseg(QXmlStreamReader &);
    void createGraph();
    void calcDst();
    void calcSpeed();
    void writeFile();
    QString calcElapsed(int &);

private slots:
    void doClose();
    void doOpen();
    void doPlot();
};

#endif // GPSPLOT_H

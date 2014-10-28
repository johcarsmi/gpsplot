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
    QVector <double> lat;
    QVector <double> lon;
    QVector <double> ele;
    QVector <double> tim;

    void processFile(const QString &);
    void process_trk(QXmlStreamReader &);
    void process_trkseg(QXmlStreamReader &);
    void trialPlot();
    void createGraph();
    PlotData *pData;
    GpGraph *ggForm;

private slots:
    void doClose();
    void doOpen();
};

#endif // GPSPLOT_H

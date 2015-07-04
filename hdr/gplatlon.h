#ifndef GPLATLON_H
#define GPLATLON_H

#include <QDialog>

#include <hdr/plotdata.h>
#include <hdr/filedownloader.h>

namespace Ui {
class GpLatLon;
}

struct edges {
    double iMinLat;
    double iMaxLat;
    double iMinLon;
    double iMaxLon;
};

class GpLatLon : public QDialog
{
    Q_OBJECT

protected:
    void resizeEvent(QResizeEvent *);

public:
    explicit GpLatLon(QWidget *parent = 0);
    ~GpLatLon();

    void ggAddData (PlotData *);
    void ggLayout();
    QImage bgImage;
    QVector<QPoint> *trkPlot;   // For track in pixel coordinates.
    QColor trkCol;

private:
    Ui::GpLatLon *ui;
    PlotData *ggData;
    FileDownloader *bgImgData;
    edges lims;
    QPoint trkPt;
    QPalette iPalS;
    QPalette iPalE;
    QString bgType;
    double _lat;    // Latitude of centre of map
    double _lon;    // Longitude of centre of map
    int _zoom;      // Zoom level of map
    int pwH;    // Plot window height
    int pwW;    // Plot window width

    void doResize();
    edges calcLimits(double & cLat, double & cLon, int iZoom, int iHgt, int iWdth);
    void calcLinePoints();
    void fireOffRequest();

private slots:
    void doClose();
    void loadBG();
    void doMap();
    void doSat();

};

#endif // GPLATLON_H
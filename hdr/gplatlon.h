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
    int pwH;    // Plot window height
    int pwW;    // Plot window width

private:
    Ui::GpLatLon *ui;
    PlotData *ggData;
    FileDownloader *bgImgData;
    edges lims;
    QPoint trkPt;
    QPalette iPalS;
    QPalette iPalE;

    void doResize();
    edges calcLimits(double & cLat, double & cLon, int iZoom, int iHgt, int iWdth);
    void calcLinePoints();

private slots:
    void doClose();
    void loadBG();

};

#endif // GPLATLON_H

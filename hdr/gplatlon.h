#ifndef GPLATLON_H
#define GPLATLON_H

#include <qt5/QtWidgets/QDialog>

#include <hdr/plotdata.h>
#include <hdr/filedownloader.h>
#include <hdr/gploading.h>

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
    void setPaintBG(bool);
    void saveDragStart();
    void passDragPos(QPoint *);
    QImage bgImage;
    QVector<QPoint> *trkPlot;   // For track in pixel coordinates.
    QColor trkCol;
    ArrowData *arrD;
    QColor arrCol;

public slots:
    void doZin();
    void doZout();

private:
    Ui::GpLatLon *ui;
    PlotData *ggData;
    FileDownloader *bgImgData;
    edges lims;
    edges limsSaved;
    QPalette iPalS;
    QPalette iPalE;
    QString bgType;
    double _lat;    // Latitude of centre of map
    double _lon;    // Longitude of centre of map
    double _latSaved;    // Latitude of centre of map at start of drag
    double _lonSaved;    // Longitude of centre of map at start of drag
    int _zoom;      // Zoom level of map
    int _zoomInit;  // Initial zoom level
    int pwH;        // Plot window height
    int pwW;        // Plot window width
    GpLoading *wLoad;
    bool doPaint;

    void doResize();
    edges calcLimits(double & cLat, double & cLon, int iZoom, int iHgt, int iWdth);
    void calcLinePoints(edges&, QVector<QPoint>*);
    void fireOffRequest(double&, double&, int&, int&, int&, QString&);
    void calcArrowPoints(PlotData *, ArrowData *);
    void calcCentrePoint();

private slots:
    void doClose();
    void loadBG();
    void drawPlot();
    void doMap();
    void doSat();
    void doZreset();
    void calculateLimits();
    void doPup();
    void doPdown();
    void doPleft();
    void doPright();
    void doPctr();

};

#endif // GPLATLON_H

#include "hdr/gplatlon.h"
#include "ui_gplatlon.h"

#include <QtDebug>

#include <math.h>


GpLatLon::GpLatLon(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GpLatLon)
{
    ui->setupUi(this);
    trkPlot = new QVector<QPoint>();
}

GpLatLon::~GpLatLon()
{
    delete ui;
//    delete trkPlot;
}

void GpLatLon::resizeEvent(QResizeEvent *e)   // Trap the form resize event to allow the graph to be resized to match.
{
    QDialog::resizeEvent(e);
    doResize();
}

void GpLatLon::doClose()
{
    this->close();
}

void GpLatLon::ggAddData(PlotData *inData)
{
    ggData = inData;
}

void GpLatLon::ggLayout()
{

    ui->dspTrkName->setText(ggData->trkName);
    ui->dspTrkDate->setText(ggData->trkDate);

    doGetBackground();
}

void GpLatLon::doGetBackground()
{
    double _lat = ((ggData->yLo + ggData->yHi) / 2);
    double _lon = ((ggData->xLo + ggData->xHi) / 2);
    int _zoom = 13;
    lims = calcLimits(_lat, _lon, _zoom, 640, 640);
    QUrl bgUrl(tr("https://maps.googleapis.com/maps/api/staticmap?center=%1,%2&zoom=%3&size=640x640&maptype=satellite&style=lightness:20").arg(_lat).arg(_lon).arg(_zoom));
    bgImgData = new FileDownloader(bgUrl, this);
    connect(bgImgData, SIGNAL(downloaded()), this, SLOT (loadBG()));
    calcLinePoints();
}

void GpLatLon::loadBG()  // When the download is complete repaint the plot area with a Google map background and the curve in cyan.
{
    bgImage.loadFromData(bgImgData->downloadedData());
//  bgBrush = QBrush(bgImage);
    this->repaint();
}

void GpLatLon::doResize()
{
//    ui->gllPlot->replot();
}

edges GpLatLon::calcLimits(double &cLat, double &cLon, int iZoom, int iHgt, int iWdth)
{
    edges el;
    double cpx, cpy, mpx, mpy;
    double wx = 0 - (iWdth / 2);
    double wy = 0 - (iHgt / 2);
    double s = qBound(-0.9999, sin(cLat * (M_PI / 180)), 0.9999);
    int tiles = 1 << iZoom;

    cpx = 128.0 + (cLon * (256.0 / 360.0));
    cpy = 128.0 + (0.5 * log((1 + s) / (1 - s)) * -(256.0 / (2.0 * M_PI)));
    mpx = (cpx * tiles) + wx;
    mpy = (cpy * tiles) + wy;
    el.iMaxLat = (2 * atan(exp(((mpy/tiles) - 128.0) / -(256.0 / (2.0 * M_PI)))) - (M_PI / 2.0)) / (M_PI / 180.0);
    el.iMinLon = (((mpx/tiles) - 128.0) / (256.0 / 360.0));

    wx = iWdth - (iWdth / 2);
    wy = iHgt - (iHgt / 2);
    mpx = (cpx * tiles) + wx;
    mpy = (cpy * tiles) + wy;
    el.iMinLat = (2 * atan(exp(((mpy/tiles) - 128.0) / -(256.0 / (2.0 * M_PI)))) - (M_PI / 2.0)) / (M_PI / 180.0);
    el.iMaxLon = (((mpx/tiles) - 128.0) / (256.0 / 360.0));
    qDebug() << el.iMinLat << el.iMinLon << el.iMaxLat << el.iMaxLon << cLat << cLon;
    return el;
}

void GpLatLon::calcLinePoints()
{
    double hLat = lims.iMaxLat - lims.iMinLat;
    double wLon = lims.iMaxLon - lims.iMinLon;
    int ix = 0;
    while (ix < ggData->xData.size())
    {
        trkPt.setX(640 * (ggData->xData[ix] - lims.iMinLon) / wLon);
        trkPt.setY(640 * (lims.iMaxLat - ggData->yData[ix]) / hLat);
        trkPlot->append(trkPt);
        ++ix;
    }
}

#include "hdr/gplatlon.h"
#include "ui_gplatlon.h"

#include <QtDebug>

#include <math.h>

#include "hdr/latlon.h"


GpLatLon::GpLatLon(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GpLatLon)
{
    ui->setupUi(this);
    trkPlot = new QVector<QPoint>();        // Initialise track plot pixel vector
    pwW = ui->gllPlot->width();             // Get width and height of plot area.
    pwH = ui->gllPlot->height();
    iPalS = ui->strtIco->palette();         // Create a red square as a key for the start point.
    iPalS.setColor(QPalette::Window ,Qt::red);
    ui->strtIco->setAutoFillBackground(true);
    ui->strtIco->setPalette(iPalS);
    iPalE = ui->strtIco->palette();         // Create a blue square as a key for the end point.
    iPalE.setColor(QPalette::Window ,Qt::blue);
    ui->endIco->setAutoFillBackground(true);
    ui->endIco->setPalette(iPalE);
    bgType = "satellite";                   // Set default view and track colour.
    trkCol = Qt::cyan;
    wLoad = 0;
    arrD = new ArrowData;
}

GpLatLon::~GpLatLon()
{
    delete ui;
}

void GpLatLon::resizeEvent(QResizeEvent *e)   // Trap the form resize event to allow the graph to be resized to match.
{   // Does nothing here - as yet!
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

void GpLatLon::ggLayout()   //
{
    ui->dspTrkName->setText(ggData->trkName);
    ui->dspTrkDate->setText(ggData->trkDate);
    // Calculate centre of plot.
    _lat = ((ggData->yLo + ggData->yHi) / 2);
    _lon = ((ggData->xLo + ggData->xHi) / 2);
    // Set zoom level for call to Google. 13 is an arbitary number that seems to work!
    _zoom = 13;
    // Calculate the limits of the downloaded map - used for plotting track later.
    lims = calcLimits(_lat, _lon, _zoom, pwW, pwH);
    // Convert lat/lon positions into pixel points for plotting track.
    calcLinePoints(lims, trkPlot);
    // Fire off map request.
    fireOffRequest(_lat, _lon, _zoom, pwW, pwH, bgType);
    calcArrowPoints(ggData, arrD, lims);
}

void GpLatLon::fireOffRequest(double &inLat, double &inLon, int &inZoom, int &inPw, int &inPh, QString &inType)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (wLoad == 0)
    {   // Pop up a loading message.
        //qDebug() << this->x() << this->y();
        wLoad = new GpLoading(this);
        wLoad->move(this->x() + 300, this->y() + 300);
        wLoad->raise();
        wLoad->show();
    }
    QUrl bgUrl(tr("https://maps.googleapis.com/maps/api/staticmap?center=%1,%2&zoom=%3&size=%4x%5&maptype=%6&style=lightness:20")\
               .arg(inLat).arg(inLon).arg(inZoom).arg(inPw).arg(inPh).arg(inType));
    bgImgData = new FileDownloader(bgUrl, this);
    connect(bgImgData, SIGNAL(downloaded()), this, SLOT (loadBG()));
}

void GpLatLon::loadBG()  // When the download is complete paint the plot area with a Google map.
{
    bgImage.loadFromData(bgImgData->downloadedData());
    this->repaint();    // Also causes repaint of the subordinate GpMapPlot.
    QApplication::restoreOverrideCursor();
    delete wLoad;
    wLoad = 0;
}

void GpLatLon::doResize()
{
//    return;
}

edges GpLatLon::calcLimits(double &cLat, double &cLon, int iZoom, int iHgt, int iWdth)
{   // This is a Qt/C++ version of the Google javascript code for working out the limits of a google map image.
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
//    qDebug() << el.iMinLat << el.iMinLon << el.iMaxLat << el.iMaxLon << cLat << cLon;
    return el;
}

void GpLatLon::calcLinePoints(edges &inLim, QVector<QPoint> *ptrPtVec) // Convert lat/lon to the relevant pixel coordinates.
{
    QPoint trkPt;
    double hLat = inLim.iMaxLat - inLim.iMinLat;
    double wLon = inLim.iMaxLon - inLim.iMinLon;
    int ix = 0;
    while (ix < ggData->xData.size())
    {
        trkPt.setX(pwW * (ggData->xData[ix] - inLim.iMinLon) / wLon);
        trkPt.setY(pwH * (inLim.iMaxLat - ggData->yData[ix]) / hLat);
        ptrPtVec->append(trkPt);
        ++ix;
    }
}

void GpLatLon::doMap()  // Plot against Google Maps background using a magenta pen.
{
    if ( (ui->rbMap->isChecked()) && (bgType != "map"))
    {
        bgType = "map";
        trkCol = Qt::magenta;
        fireOffRequest(_lat, _lon, _zoom, pwW, pwH, bgType);
    }
}

void GpLatLon::doSat()  // Plot against Google Earth background using a cyan pen.
{
    if ((ui->rbSat->isChecked()) && (bgType != "satellite"))
    {
        bgType = "satellite";
        trkCol = Qt::cyan;
        fireOffRequest(_lat, _lon, _zoom, pwW, pwH, bgType);
    }
}

void GpLatLon::calcArrowPoints(PlotData *inPlot, ArrowData *outArr, edges &inLim)
{
    int stepsToAvg = 2;     // The number of points before and after the selected point to use to get track direction.
    int arrSteps = 100;     // The spacing of arrows along the track.
    QPointF headPt;
    QPointF fPt;
    QPointF bPt;
    QPoint lPt = QPoint(0, 0);
    QPoint rPt = QPoint(0, 0);
    double dirctn = 0.0;
    // Set up array of arrow head points and adjacent points for calculation direction
    for (int ix = (arrSteps / 2);
         (ix < (inPlot->xData.count() - stepsToAvg - (arrSteps / 2)));
         ix += arrSteps)
    {
        headPt.setX(inPlot->xData[ix]);
        headPt.setY(inPlot->yData[ix]);
        bPt.setX(inPlot->xData[ix - stepsToAvg]);
        bPt.setY(inPlot->yData[ix - stepsToAvg]);
        fPt.setX(inPlot->xData[ix + stepsToAvg]);
        fPt.setY(inPlot->yData[ix + stepsToAvg]);
        outArr->appendRow(headPt, bPt, fPt, dirctn, trkPlot->at(ix), lPt, rPt);
    }
    // Update array with direction information.
    for (int ix = 0; ix < outArr->count(); ++ix)
    {
        outArr->dirctn[ix] = angleFromCoordinates(outArr->headB[ix].y(), outArr->headB[ix].x(),
                                                  outArr->headF[ix].y(), outArr->headF[ix].x());
    }
    // Calculate end points of arrow lines.
    for (int ix = 0; ix < outArr->count(); ++ix)
    {
        // Calculate one set of arrow points.
        double aLen = 0.0012;     // Length of arrow line in degrees.
        double arrAngle = 30.0;     // Angle of arrow line to track.
        double hLat = inLim.iMaxLat - inLim.iMinLat;
        double wLon = inLim.iMaxLon - inLim.iMinLon;
        double eLon, eLat, eBrg;
        // Calculate arrow points in lat/lon and  Convert to pixels.
        // 'Left' arrow arm.
        eBrg = outArr->dirctn.at(ix) - 180.0 + arrAngle;
        eBrg = deg2rad(fmod((eBrg + 360.0) , 360.0));
        eLon = outArr->head.at(ix).x() + (aLen * sin(eBrg));
        eLat = outArr->head.at(ix).y() + (aLen * cos(eBrg));
        arrD->lPt[ix].setX(pwW * (eLon - inLim.iMinLon) / wLon);
        arrD->lPt[ix].setY(pwH * (inLim.iMaxLat - eLat) / hLat);
        // 'Right' arrow arm.
        eBrg = outArr->dirctn.at(ix) - 180.0 - arrAngle;
        eBrg = deg2rad(fmod((eBrg + 360) , 360.0));
        eLon = outArr->head.at(ix).x() + (aLen * sin(eBrg));
        eLat = outArr->head.at(ix).y() + (aLen * cos(eBrg));
        arrD->rPt[ix].setX(pwW * (eLon - inLim.iMinLon) / wLon);
        arrD->rPt[ix].setY(pwH * (inLim.iMaxLat - eLat) / hLat);
    }
}

#include "hdr/gplatlon.h"
#include "ui_gplatlon.h"

#include <qt5/QtCore/QtDebug>

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
    arrCol = Qt::yellow;
    wLoad = 0;
    arrD = new ArrowData;
    doPaint = true;
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
   calcCentrePoint();
    // Set zoom level for call to Google. 17 is an arbitary starting point.
    _zoom = 19;
    do          // Keep decreasing the scale until the plot fits in the map.
    {
        _zoom--;
        calculateLimits();
    }
    while (lims.iMaxLat < ggData->yHi || lims.iMinLat > ggData->yLo
           || lims.iMaxLon < ggData->xHi || lims.iMinLon > ggData->xLo);
    _zoomInit = _zoom;
    drawPlot();
}

void GpLatLon::fireOffRequest(double &inLat, double &inLon, int &inZoom, int &inPw, int &inPh, QString &inType)
{
    if (!doPaint) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (wLoad == 0)
    {   // Pop up a loading message.
        //qDebug() << this->x() << this->y();
        wLoad = new GpLoading(this);
        wLoad->move(this->x() + 300, this->y() + 300);
        wLoad->raise();
        wLoad->show();
    }
    QUrl bgUrl(tr("https://maps.googleapis.com/maps/api/staticmap?center=%1,%2&zoom=%3&size=%4x%5&maptype=%6&style=lightness:20&key=AIzaSyDs0p8FXnpbCIYs5fsM9B02C5xjWy00DwE")\
               .arg(inLat).arg(inLon).arg(inZoom).arg(inPw).arg(inPh).arg(inType));
    bgImgData = new FileDownloader(bgUrl, this);
    connect(bgImgData, SIGNAL(downloaded()), this, SLOT (loadBG()));
}

void GpLatLon::loadBG()  // When the download is complete, paint the plot area with a Google map.
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
    ptrPtVec->clear();
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
        arrCol = Qt::blue;
        fireOffRequest(_lat, _lon, _zoom, pwW, pwH, bgType);
    }
}

void GpLatLon::doSat()  // Plot against Google Earth background using a cyan pen.
{
    if ((ui->rbSat->isChecked()) && (bgType != "satellite"))
    {
        bgType = "satellite";
        trkCol = Qt::cyan;
        arrCol = Qt::yellow;
        fireOffRequest(_lat, _lon, _zoom, pwW, pwH, bgType);
    }
}

void GpLatLon::calcArrowPoints(PlotData *inPlot, ArrowData *outArr)
{
    outArr->clear();
    int stepsToAvg = 9;     // The number of points before and after the selected point to use to get track direction.
    int arrSteps = 100;     // The spacing of arrows along the track.
    QPointF headPt;         // The track point at the head of the arrow.
    QPointF fPt;            // The track point in front of the head point use for direction determination.
    QPointF bPt;            // The track point behind the head point use for direction determination.
    QPoint lPt = QPoint(0, 0);      // What will be the end of the left arrow line.
    QPoint rPt = QPoint(0, 0);      // What will be the end of the right arrow line.
    double dirctn = 0.0;
    // Set up array of arrow head points and adjacent points for calculation direction
    for (int ix = (arrSteps / 2);
         (ix < (inPlot->xData.count() - stepsToAvg - (arrSteps / 2)));  // Make sure that all points will be within vector bounds.
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
    double arrLen = 9;          // Length of arrow line in pixels.
    double arrAngle = 30.0;     // Angle of arrow line to track.
    double eBrg;
    for (int ix = 0; ix < outArr->count(); ++ix)
    {
        // Calculate one set of arrow points.
        // Calculate arrow points in lat/lon and  Convert to pixels.
        // 'Left' arrow arm.
        eBrg = outArr->dirctn[ix] - 180.0 - arrAngle;  // degrees
        eBrg = deg2rad(fmod((eBrg + 360.0) , 360.0));  // radians
        arrD->lPt[ix].setX(arrD->trkPt[ix].x() + (arrLen * sin(eBrg)));
        arrD->lPt[ix].setY(arrD->trkPt[ix].y() - (arrLen * cos(eBrg)));
        // 'Right' arrow arm.
        eBrg = outArr->dirctn[ix] - 180.0 + arrAngle;
        eBrg = deg2rad(fmod((eBrg + 360.0) , 360.0));
        arrD->rPt[ix].setX(arrD->trkPt[ix].x() + (arrLen * sin(eBrg)));
        arrD->rPt[ix].setY(arrD->trkPt[ix].y() - (arrLen * cos(eBrg)));
    }
}

void GpLatLon::calculateLimits()
{
    // Calculate the limits of the downloaded map - used for plotting track later.
    lims = calcLimits(_lat, _lon, _zoom, pwW, pwH);
}

void GpLatLon::calcCentrePoint()
{
    // Calculate centre of plot.
    _lat = ((ggData->yLo + ggData->yHi) / 2.0);
    _lon = ((ggData->xLo + ggData->xHi) / 2.0);
}

void GpLatLon::drawPlot()
{
    // Convert lat/lon positions into pixel points for plotting track.
    calcLinePoints(lims, trkPlot);
    calcArrowPoints(ggData, arrD);
    // Fire off map request.
    fireOffRequest(_lat, _lon, _zoom, pwW, pwH, bgType);
}

void GpLatLon::doZin()
{
    _zoom++;
    calculateLimits();
    drawPlot();
}

void GpLatLon::doZout()
{
    _zoom--;
    calculateLimits();
    drawPlot();
}

void GpLatLon::doZreset()
{
    _zoom = _zoomInit;
    calculateLimits();
    drawPlot();
}

void GpLatLon::doPup()
{
    _lat = _lat + ((lims.iMaxLat - lims.iMinLat) / 3);
    calculateLimits();
    drawPlot();
}

void GpLatLon::doPdown()
{
    _lat = _lat - ((lims.iMaxLat - lims.iMinLat) / 3);
    calculateLimits();
    drawPlot();
}

void GpLatLon::doPleft()
{
    _lon = _lon - ((lims.iMaxLon - lims.iMinLon) / 3);
    calculateLimits();
    drawPlot();
}

 void GpLatLon::doPright()
 {
     _lon = _lon + ((lims.iMaxLon - lims.iMinLon) / 3);
     calculateLimits();
     drawPlot();
 }

 void GpLatLon::doPctr()
 {
     calcCentrePoint();
     calculateLimits();
     drawPlot();
 }

 void GpLatLon::setPaintBG(bool state)
 {
     doPaint = state;
 }

 void GpLatLon::saveDragStart()
 {
     _latSaved = _lat;
     _lonSaved = _lon;
     limsSaved = lims;
 }

 void GpLatLon::passDragPos(QPoint *curPt)
 {
    // Calculate how much the pixel movement means in terms of Lat and Lon.
    // The difference in signs in the next two lines reflects the y=0 being at the top of the picture.
    _lat = _latSaved + ((limsSaved.iMaxLat - limsSaved.iMinLat) * ((double)curPt->y() / (double)ui->gllPlot->height()) );
    _lon = _lonSaved - ((limsSaved.iMaxLon - limsSaved.iMinLon) * ((double)curPt->x() / (double)ui->gllPlot->width()) );
    //qDebug("GpLatLon: _lat = %f : _lon = %f", _lat, _lon);
    calculateLimits();
    drawPlot();
    ui->gllPlot->repaint();
 }

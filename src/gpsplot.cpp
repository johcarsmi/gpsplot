#include "gpsplot.h"
#include "ui_gpsplot.h"
#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QXmlStreamAttributes>
#include <QVector>

#include <qwt_date.h>

#include "hdr/latlon.h"

GpsPlot::GpsPlot(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GpsPlot)
{
    ui->setupUi(this);
    pData = new PlotData();
    ui->rbUndef0->setVisible(false);
    ui->rbUndef1->setVisible(false);
    ui->pbPlot->setEnabled(false);
}

GpsPlot::~GpsPlot()
{
    delete ui;
    delete pData;
}

void GpsPlot::doOpen()
{
    fileName = QFileDialog::getOpenFileName(this, tr("Open track file"), QDir::homePath(), tr("GPS track files (*.gpx)"));
    if (fileName != "")
    {
        processFile(fileName);
        if (!(tim.isEmpty())) ui->pbPlot->setEnabled(true);
    }
}

void GpsPlot::processFile(const QString & inFile)
{
    QFile gpxFile(inFile);
    if (!gpxFile.open(QFile::ReadOnly | QFile::Text ) )
    {
        QMessageBox::warning(this, tr("Open File error"), tr("Error: %1").arg(gpxFile.errorString()));
        return;
    }
    xRead.setDevice(&gpxFile);
    if (xRead.readNextStartElement())               // Check this is a gpx file.
        {
            if (xRead.name() != tr("gpx")) {
                xRead.raiseError(tr("Not a gpx file"));
                return; }
        }
        while (xRead.readNextStartElement())        // Skip unwanted elements.
        {
//          qDebug() << xRead.name();               // DEBUG
            if (xRead.name() == tr("trk")) break;
            xRead.skipCurrentElement();
        }
        qDebug() << xRead.name();                   // DEBUG
        process_trk(xRead);
        gpxFile.close();
 }

void GpsPlot::process_trk(QXmlStreamReader & inXml)
{
    inXml.readNextStartElement();
    qDebug() << inXml.name();                       // DEBUG
    eleText = inXml.readElementText(QXmlStreamReader::SkipChildElements);
    qDebug() << eleText;
    ui->dspTrackName->setText(eleText);
    while (inXml.readNextStartElement())        // Skip unwanted elements.
    {
        if (inXml.name() == tr("trkseg")) break;
        inXml.skipCurrentElement();
    }
    qDebug() << inXml.name();                   // DEBUG
    process_trkseg(inXml);
}

void GpsPlot::process_trkseg(QXmlStreamReader & inXml)
{
    firsttime = true;
    tim.clear();
    ele.clear();
    lat.clear();
    lon.clear();
    dst.clear();
    hspd.clear();
    vspd.clear();
    QXmlStreamAttributes attr;
    QDateTime qdt;
    while (inXml.readNext() != inXml.atEnd())
    {
        if (inXml.isStartElement())
        {
            // qDebug()  << inXml.tokenString() << inXml.name();                   // DEBUG
            if (inXml.name() == tr("trkseg"))
            {
                continue;
            }
            if (inXml.name() == tr("trkpt"))
            {
                attr = inXml.attributes();
                lat.append(QString(attr.value("lat").toString()).toDouble(&dok));
                lon.append(QString(attr.value("lon").toString()).toDouble(&dok));
            }
            if (inXml.name() == tr("ele"))
            {
                ele.append(inXml.readElementText().toDouble(&dok));
            }
            if (inXml.name() == tr("time"))
            {
                qdt = QDateTime::fromString(inXml.readElementText(), Qt::ISODate);
                if (firsttime) trkDate = qdt.toString(Qt::SystemLocaleShortDate).left(10);
                tim.append(QwtDate::toDouble(qdt));
            }
        }
    }
    calcDst();
}

void GpsPlot::calcDst()
{
    dst.append(0.0);
    double cum = 0.0;
    for (int ix = 1; ix < lat.count(); ix++)
    {
        cum += distance(lat[ix -1],lon[ix-1],lat[ix],lon[ix],'K');
        dst.append(cum);
    }
    calcSpeed2();
}

void GpsPlot::calcSpeed()
{
     double wrk = 0.0;
     hspd.append(0.0);
     vspd.append(0.0);
     for (int ix = 1; ix < dst.count(); ix++)
     {
         wrk = ( (dst[ix] - dst[ix-1]) * 3600000 ) / (tim[ix] - tim[ix-1]);
         hspd.append(wrk);  // Horizontal Speed in kmh
         wrk = ( (ele[ix] - ele[ix-1]) * 60000 ) / (tim[ix] - tim[ix-1]);
         vspd.append(wrk);  // Vertical Speed in m/min
     }
}

void GpsPlot::calcSpeed2()
{
     double wrk = 0.0;
     hspd.append(0.0);
     vspd.append(0.0);
     for (int ix = 8; ix < dst.count(); ix++)
     {
         wrk = ( (dst[ix] - dst[ix-8]) * 3600000 ) / (tim[ix] - tim[ix-8]);
         hspd.append(wrk);  // Horizontal Speed in kmh
         wrk = ( (ele[ix] - ele[ix-8]) * 60000 ) / (tim[ix] - tim[ix-8]);
         vspd.append(wrk);  // Vertical Speed in m/min
     }
}

void GpsPlot::doPlot()
{
    // Common data to all plots.
    pData->trkName = ui->dspTrackName->text();
    pData->trkDate =trkDate;
    // Choose plot from radio button and set up data.
    if (ui->rbAltTime->isChecked())
    {
        // Time / Elevation.
        pData->xData = tim;
        pData->yData = ele;
        pData->xLabel = "Time";
        pData->yLabel = "Elevation (m)";
        pData->xType = "datetime";
        pData->yType = "double";
    }
    else if (ui->rbAltDist->isChecked())
    {
        // Distance / Elevation.
        pData->xData = dst;
        pData->yData = ele;
        pData->xLabel = "Distance (Kilometers)";
        pData->yLabel = "Elevation (m)";
        pData->xType = "double";
        pData->yType = "double";
    }
    else if (ui->rbDistTim->isChecked())
    {
        // Distance / Time.
        pData->xData = tim;
        pData->yData = dst;
        pData->xLabel = "Time";
        pData->yLabel = "Distance (Kilometers)";
        pData->xType = "datetime";
        pData->yType = "double";
    }
    else if (ui->rbLatLon->isChecked())
    {
        // Lat / Lon // TODO - Fix scaling to be the same on both axes.
        pData->xData = lon;
        pData->yData = lat;
        pData->xLabel = "Longitude";
        pData->yLabel = "Latitude";
        pData->xType = "double";
        pData->yType = "double";
    }
    else if (ui->rbHspdTim->isChecked())
    {
        pData->xData = tim;
        pData->yData = hspd;
        pData->xLabel = "Time";
        pData->yLabel = "Horzontal Speed (kmh)";
        pData->xType = "datetime";
        pData->yType = "double";
    }
    else if (ui->rbVspdTim->isChecked())
    {
        pData->xData = tim;
        pData->yData = vspd;
        pData->xLabel = "Time";
        pData->yLabel = "Vertical Speed (m per minute)";
        pData->xType = "datetime";
        pData->yType = "double";
    }
    // Plot requested Graph
    GpGraph *gg = new GpGraph(this);
    gg->ggAddData(pData);
    gg->ggLayout();
    gg->exec();
    delete gg;

}

void GpsPlot::doClose()
{
    this->close();
}

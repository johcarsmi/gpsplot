#include "gpsplot.h"
#include "ui_gpsplot.h"
#include <qt5/QtCore/QDateTime>
#include <qt5/QtCore/QDebug>
#include <qt5/QtWidgets/QFileDialog>
#include <qt5/QtWidgets/QMessageBox>
#include <qt5/QtCore/QXmlStreamAttributes>
#include <qt5/QtCore/QVector>
#include <math.h>
#include <stdio.h>

#include <qwt6/qwt_date.h>

#include "hdr/latlon.h"
#include "hdr/gplatlon.h"

GpsPlot::GpsPlot(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GpsPlot)
{
    ui->setupUi(this);
    pData = new PlotData();
    ui->pbPlot->setEnabled(false);
    settings = new QSettings(QDir::homePath() + QDir::separator() + stgsFile, QSettings::IniFormat, this);
    curDir = settings->value("directory", QDir::homePath() + QDir::separator()).toString();
}

GpsPlot::~GpsPlot()
{
    delete ui;
    delete pData;
}

void GpsPlot::doOpen()  // Prompt the use with a FileOpen dialog and process the file selected.
{
    fileName = QFileDialog::getOpenFileName(this, tr("Open track file"), curDir, tr("GPS track files (*.gpx)"));
    if (fileName != "")
    {
        if (!processFile(fileName)) return;
        if (!(tim.isEmpty())) ui->pbPlot->setEnabled(true);
    curDir = fileName;
    int ix = curDir.count() - 1;
    while (!curDir.endsWith('/'))
    {
        curDir.remove(ix, 1);
        --ix;
    }
    settings->setValue("directory", curDir);
    }
}

bool GpsPlot::processFile(const QString & inFile)   // Create XmlStreamReader and
{
    bool trkFound = false;
    QFile gpxFile(inFile);
    if (!gpxFile.open(QFile::ReadOnly | QFile::Text ) )
    {
        QMessageBox::warning(this, tr("Open File error"), tr("Error: %1").arg(gpxFile.errorString()));
        return trkFound;
    }
    xRead.setDevice(&gpxFile);
    if (xRead.readNextStartElement())               // Check this is a gpx file.
        {
            if (xRead.name().toString() != "gpx")
            {
                QMessageBox::warning(this, tr("Error"), tr("Not a gpx file!"));
                xRead.raiseError(tr("Not a gpx file"));
                gpxFile.close();
                return trkFound;
            }
        }
    attr = xRead.attributes();
    creatorDevice = QString(attr.value("creator").toString());
    while (xRead.readNextStartElement())        // Skip unwanted elements stopping on the 'trk' element.
        {
            if (creatorDevice.startsWith("OsmAnd") && xRead.name() == "metadata")   // Get track name for OsmAnd ... .
            {
                while (xRead.readNextStartElement())
                {
                    if (xRead.name() == "name")
                    {
                        eleText = xRead.readElementText(QXmlStreamReader::SkipChildElements);
                        ui->dspTrackName->setText(eleText);
                        break;
                    }
                }
            }

//          qDebug() << xRead.name();               // DEBUG
            if (xRead.name() == "trk")
            {
                trkFound = true;
            }
            if (trkFound) break;
            xRead.skipCurrentElement();
        }
//        qDebug() << xRead.name();                   // DEBUG
    if (trkFound) process_trk();
    else
        {
            QMessageBox::warning(this, tr("Error"), tr("Not a gpx track file!"));
            xRead.raiseError(tr("Not a gpx track file"));
        }
    gpxFile.close();
    return trkFound;
 }

void GpsPlot::process_trk() // Process the 'trk' element data and process track segment elements.
{
    //qDebug() << xRead.name();                       // DEBUG
    while (xRead.readNext() != xRead.atEnd())
    {
        if (xRead.isStartElement() && xRead.name() == "name")   // Get the name of the track for Garmin Oregon.
        {
            eleText = xRead.readElementText();
            ui->dspTrackName->setText(eleText);
        }
        if (xRead.isStartElement() && xRead.name() == "trkseg")
        {
            process_trkseg();
        }
    }
    //qDebug() << xRead.name();                   // DEBUG
    elapsedTime = static_cast<int>(stTime.secsTo(qdt));   // When get here all points have been processed.
    calcDst();
    calcSpeed();
    ui->dspTrackLength->setText(tr("%1").arg(dst[dst.count() - 1]));
    ui->dspTrackTime->setText(calcElapsed(elapsedTime));
    ui->dspTrackDate->setText(trkDate);
}

void GpsPlot::process_trkseg()
{
    firsttime = true;
    tim.clear();
    ele.clear();
    lat.clear();
    lon.clear();
    dst.clear();
    hspd.clear();
    vspd.clear();
    latMax = latMin = lonMax = lonMin = 0;
    while (xRead.readNext() != xRead.atEnd())
    {
        if (xRead.isStartElement())
        {
            // qDebug()  << xRead.tokenString() << xRead.name();                   // DEBUG
/*            if (xRead.name() == "trkseg")       // <<<<<< this block probably not required.
            {   // Ignore track segments and process only track points.
                continue;
            }   */
            if (xRead.name() == "trkpt")    // Only process trkpt elements within a trkseg
            {
                process_trkpt();
            }
         }
    }
}

void GpsPlot::process_trkpt()
{
    // Extract the lat and lon.
    attr = xRead.attributes();
    lat.append(QString(attr.value("lat").toString()).toDouble(&dok));
    lon.append(QString(attr.value("lon").toString()).toDouble(&dok));
    if (!firsttime) // Get the minimum and maximum values to allow equal scaling later.
    {
        if (lat.last() < latMin) latMin = lat.last();
        if (lat.last() > latMax) latMax = lat.last();
        if (lon.last() < lonMin) lonMin = lon.last();
        if (lon.last() > lonMax) lonMax = lon.last();
    }
    else
    {
        latMin = latMax = lat.last();
        lonMin = lonMax = lon.last();
    }
    while (xRead.readNext() != xRead.atEnd())
    {
        if (xRead.isEndElement())
        {
            if (xRead.name() == "trkpt") return;    // If end of trkpt end this loop
        }
        if (xRead.name() == "ele")     // Extract the elevation.
        {
            ele.append(xRead.readElementText().toDouble(&dok));
        }
        else if (xRead.name() == "time")   // Extract the date/time and convert to a QwtDate.
        {
            qdt = QDateTime::fromString(xRead.readElementText(), Qt::ISODate);
            if (firsttime)
            {
                stTime = qdt;
                trkDate = qdt.toString(Qt::SystemLocaleShortDate).left(10);
                firsttime = false;
            }
            tim.append(QwtDate::toDouble(qdt));
        }
    }
}

void GpsPlot::calcDst()
{
    dst.append(0.0);
    double cum = 0.0;
    double dval = 0.0;
    bool nomore = true;     // triggers debug code below when set to false.
    for (int ix = 1; ix < lat.count(); ix++)
    {
        dval = distance(lat[ix -1],lon[ix-1],lat[ix],lon[ix],'K');
        if (isnan(dval))
        {
            if(!nomore)
            {
            qDebug() << printf("lat-1: %3.10f | lat: %3.10f | lon-1: %3.10f | lon: %3.10f\n",
                   lat[ix-1], lat[ix], lon[ix-1], lon[ix]);
            nomore = true;
            }
        }
        else
        {
            cum += dval;
            dst.append(cum);
        }
    }
}

void GpsPlot::calcSpeed()   // Taking each reading 120 seconds apart.
{   // The error on the position recorded - especially the elevation - means that speed between adjacent points are meaningless.
     double wrk = 0.0;
     int ixh = 0;
     hspd.append(0.0);
     vspd.append(0.0);
     tim2.append(tim[0]);
     for (int ix = 1; ix < dst.count(); ix++)
     {
         if (tim[ix] - tim[ixh] > 120000)   // Values in tim are in milliseconds. 120 seconds gave a reasonable result.
         {
             wrk = ( (dst[ix] - dst[ixh]) * 3600000 ) / (tim[ix] - tim[ixh]);
             hspd.append(wrk);  // Horizontal Speed in kmh
             wrk = ( (ele[ix] - ele[ixh]) * 60000 ) / (tim[ix] - tim[ixh]);
             vspd.append(wrk);  // Vertical Speed in m/min
             tim2.append(tim[ix]);
             ixh = ix;
         }
     }
     // qDebug() << ixh;
}

QString GpsPlot::calcElapsed(int & inSecs)  // Returns a string of hours and minutes from an input of seconds.
{
    QString oStr;
    int wrk = inSecs;
    wrk = wrk / 60;
    oStr = tr(" %1 min").arg(wrk % 60);
    wrk = wrk / 60;
    oStr = tr(" %1 hrs").arg(wrk) + oStr;
    return oStr;
}

void GpsPlot::doPlot()
{
    // Common data to all plots.
    pData->trkName = ui->dspTrackName->text();
    pData->trkDate = trkDate;
    pData->manScale = false;
    pData->latlon = false;
    // Choose plot from radio button and set up data.
    if (ui->rbAltTime->isChecked())
    {
        // Time / Elevation.
        pData->xData = tim;
        pData->yData = ele;
        pData->xLabel = tr("Time (UTC)");
        pData->yLabel = tr("Elevation (m)");
        pData->xType = "datetime";
        pData->yType = "double";
    }
    else if (ui->rbAltDist->isChecked())
    {
        // Distance / Elevation.
        pData->xData = dst;
        pData->yData = ele;
        pData->xLabel = tr("Distance (Kilometers)");
        pData->yLabel = tr("Elevation (m)");
        pData->xType = "double";
        pData->yType = "double";
    }
    else if (ui->rbDistTim->isChecked())
    {
        // Distance / Time.
        pData->xData = tim;
        pData->yData = dst;
        pData->xLabel = "Time (UTC)";
        pData->yLabel = "Distance (Kilometers)";
        pData->xType = "datetime";
        pData->yType = "double";
    }
    else if (ui->rbLatLon->isChecked() || ui->rbMapTrack->isChecked())
    {
        // Lat / Lon - Make scaling to be the same on both axes and centre track plot.
        pData->latlon = true;
        pData->xLo = (floor(lonMin * 100)) / 100;
        pData->xHi = (ceil(lonMax * 100)) / 100;
        xCtr = (pData->xHi + pData->xLo)/2;
        pData->yLo = (floor(latMin * 100)) / 100;
        pData->yHi = (ceil(latMax * 100)) / 100;
        yCtr = (pData->yHi + pData->yLo)/2;
        if ((pData->yHi - pData->yLo) > (pData->xHi - pData->xLo))
        {
            pData->xLo = xCtr - (pData->yHi - pData->yLo) / 2;
            pData->xHi = xCtr + (pData->yHi - pData->yLo) / 2;
        }
        else
        {
            pData->yLo = yCtr - (pData->xHi - pData->xLo) / 2;
            pData->yHi = yCtr + (pData->xHi - pData->xLo) / 2;
        }
        pData->manScale = true;
        pData->xData = lon;
        pData->yData = lat;
        pData->xLabel = tr("Longitude");
        pData->yLabel = tr("Latitude");
        pData->xType = "double";
        pData->yType = "double";
        if(ui->rbMapTrack->isChecked())     // Want plot on picture not graph.
        {
            GpLatLon *gl = new GpLatLon(this);
            gl->ggAddData(pData);
            gl->ggLayout();
            gl->exec();
            delete gl;
            return;
        }
    }
    else if (ui->rbHspdTim->isChecked())
    {
        pData->xData = tim2;
        pData->yData = hspd;
        pData->xLabel = tr("Time (UTC)");
        pData->yLabel = tr("Horizontal Speed (kmh)");
        pData->xType = "datetime";
        pData->yType = "double";
    }
    else if (ui->rbVspdTim->isChecked())
    {
        pData->xData = tim2;
        pData->yData = vspd;
        pData->xLabel = tr("Time (UTC)");
        pData->yLabel = tr("Vertical Speed (m per minute)");
        pData->xType = "datetime";
        pData->yType = "double";
    }
    else if (ui->rbFile->isChecked())
    {
        writeFile();
        QMessageBox::information(this, tr("Notification"), tr("File written to ~/track.txt"));
        return;
    }
    // Plot requested Graph
    GpGraph *gg = new GpGraph(this);
    gg->ggAddData(pData);
    gg->ggLayout();
    gg->exec();
    delete gg;

}

void GpsPlot::writeFile()   // Write a tab separated file with the track point data.
{
    QFile oFile(QDir::homePath() + QDir::separator() + "track.txt", this);
    if (!oFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QMessageBox::critical(this, tr("ERROR"), tr("Unable to open track.txt for writing:") + oFile.errorString());
        return;
    }
    QTextStream tStrm(&oFile);
    for (int ix = 0; ix < tim.count(); ix++)
    {
        tStrm << (QwtDate::toDateTime(tim[ix])).toString(Qt::ISODate) << "\t" << lat[ix] << "\t" << lon[ix] << "\t" << ele[ix] << Qt::endl;
    }
}

void GpsPlot::doClose()
{
    settings->sync();
    this->close();
}

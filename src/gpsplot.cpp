#include "gpsplot.h"
#include "ui_gpsplot.h"
#include <qt5/QtCore/QDateTime>
#include <qt5/QtCore/QDebug>
#include <qt5/QtWidgets/QFileDialog>
#include <qt5/QtWidgets/QMessageBox>
#include <qt5/QtCore/QXmlStreamAttributes>
#include <qt5/QtCore/QVector>
#include <math.h>

#include <qwt_date.h>

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
            if (xRead.name() != tr("gpx")) {
                QMessageBox::warning(this, tr("Error"), tr("Not a gpx file!"));
                xRead.raiseError(tr("Not a gpx file"));
                gpxFile.close();
                return trkFound; }
        }
        while (xRead.readNextStartElement())        // Skip unwanted elements stopping on the 'trk' element.
        {
//          qDebug() << xRead.name();               // DEBUG
            if (xRead.name() == tr("trk"))
            {
                trkFound = true;
                break;
            }
            xRead.skipCurrentElement();
        }
//        qDebug() << xRead.name();                   // DEBUG
        if (trkFound) process_trk(xRead);
        else
        {
            QMessageBox::warning(this, tr("Error"), tr("Not a gpx track file!"));
            xRead.raiseError(tr("Not a gpx track file"));
        }
        gpxFile.close();
        return trkFound;
 }

void GpsPlot::process_trk(QXmlStreamReader & inXml) // Process the 'trk' element data and process track segment elements.
{
    inXml.readNextStartElement();
    //qDebug() << inXml.name();                       // DEBUG
    eleText = inXml.readElementText(QXmlStreamReader::SkipChildElements);
//    qDebug() << eleText;
    ui->dspTrackName->setText(eleText);
    while (inXml.readNextStartElement())        // Skip unwanted elements.
    {
        if (inXml.name() == tr("trkseg")) break;
        inXml.skipCurrentElement();
    }
    //qDebug() << inXml.name();                   // DEBUG
    process_trkseg(inXml);
    ui->dspTrackLength->setText(tr("%1").arg(dst[dst.count() - 1]));
    ui->dspTrackTime->setText(calcElapsed(elapsedTime));
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
    latMax = latMin = lonMax = lonMin = 0;
    QXmlStreamAttributes attr;
    QDateTime qdt;
    while (inXml.readNext() != inXml.atEnd())
    {
        if (inXml.isStartElement())
        {
            // qDebug()  << inXml.tokenString() << inXml.name();                   // DEBUG
            if (inXml.name() == tr("trkseg"))
            {   // Ignore track segments and process only track points.
                continue;
            }
            if (inXml.name() == tr("trkpt"))
            {   // Extract the lat and lon.
                attr = inXml.attributes();
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
            }
            if (inXml.name() == tr("ele"))
            {   // Extract the elevation.
                ele.append(inXml.readElementText().toDouble(&dok));
            }
            if (inXml.name() == tr("time"))
            {   // Extract the date/time and convert to a QwtDate.
                qdt = QDateTime::fromString(inXml.readElementText(), Qt::ISODate);
                if (firsttime)
                {
                    stTime = qdt;
                    trkDate = qdt.toString(Qt::SystemLocaleShortDate).left(10);
                    firsttime = false;
                }
                tim.append(QwtDate::toDouble(qdt));
            }
        }
        elapsedTime = stTime.secsTo(qdt);   // When get here all points have been processed.
    }
    calcDst();
    calcSpeed();
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
        pData->xLabel = "Time (UTC)";
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
        pData->xLabel = "Longitude";
        pData->yLabel = "Latitude";
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
        pData->xLabel = "Time (UTC)";
        pData->yLabel = "Horzontal Speed (kmh)";
        pData->xType = "datetime";
        pData->yType = "double";
    }
    else if (ui->rbVspdTim->isChecked())
    {
        pData->xData = tim2;
        pData->yData = vspd;
        pData->xLabel = "Time (UTC)";
        pData->yLabel = "Vertical Speed (m per minute)";
        pData->xType = "datetime";
        pData->yType = "double";
    }
    else if (ui->rbFile->isChecked())
    {
        writeFile();
        QMessageBox::information(this, "Notification", "File written to ~/track.txt");
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
        tStrm << (QwtDate::toDateTime(tim[ix])).toString(Qt::ISODate) << "\t" << lat[ix] << "\t" << lon[ix] << "\t" << ele[ix] << endl;
    }
}

void GpsPlot::doClose()
{
    settings->sync();
    this->close();
}

#include "gpsplot.h"
#include "ui_gpsplot.h"
#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QXmlStreamAttributes>
#include <QVector>

#include <qwt_date.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
//#include <qwt_date_scale_engine.h>
#include <qwt_date_scale_draw.h>

#include <math.h>

GpsPlot::GpsPlot(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GpsPlot)
{
    ui->setupUi(this);
    pData = new PlotData();
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

    //trialPlot();
    createGraph();
}

void GpsPlot::process_trkseg(QXmlStreamReader & inXml)
{
    tim.clear();
    ele.clear();
    lat.clear();
    lon.clear();
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
                tim.append(QwtDate::toDouble(qdt));
            }
        }
    }
}

void GpsPlot::createGraph()
{
    pData->xData = tim;
    pData->yData = ele;
    pData->xLabel = "Time";
    pData->yLabel = "Elevation (m)";
    pData->xType = "datetime";
    pData->yType = "double";
    pData->trkName = ui->dspTrackName->text();
    GpGraph *gg = new GpGraph(this);
    gg->ggAddData(pData);
    gg->ggLayout();
    gg->exec();
    delete gg;
}

void GpsPlot::trialPlot()
{
    QwtPlotCurve *curv = new QwtPlotCurve();
    QwtPlotGrid *grd = new QwtPlotGrid();
    QwtDateScaleDraw *dsd = new QwtDateScaleDraw();

    ui->plotArea->setTitle(ui->dspTrackName->text());
    ui->plotArea->setCanvasBackground(Qt::white);
    ui->plotArea->setAxisTitle(QwtPlot::yLeft, "Elevation (m)");
    // TODO time on X axis.

    dsd->setDateFormat(QwtDate::Minute, QString("hh:mm"));      // Not doing what's wanted at the moment.
    ui->plotArea->setAxisScaleDraw(QwtPlot::xBottom, dsd);

    qDebug() << ((QwtDateScaleDraw*)ui->plotArea->axisScaleDraw(QwtPlot::xBottom))->dateFormat(QwtDate::Minute);
    // Shows that the plot area has the correct format string.

    //      label on X axis.
    ui->plotArea->setAxisTitle(QwtPlot::xBottom, "Time");
    ui->plotArea->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignRight);
    ui->plotArea->setAxisLabelRotation(QwtPlot::xBottom, 90.0);
    QFont fnt("Liberation Sans", 7);
    ui->plotArea->setAxisFont(QwtPlot::xBottom, fnt);

    //      distance on X axis.
    //      ability to re-draw a new graph on

    grd->attach( ui->plotArea);
    curv->setSamples(tim, ele);
    curv->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curv->attach(ui->plotArea);
    ui->plotArea->replot();
}

void GpsPlot::doClose()
{
    this->close();
}

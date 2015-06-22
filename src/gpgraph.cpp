#include "hdr/gpgraph.h"
#include "ui_gpgraph.h"

#include <qwt_date.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_date_scale_draw.h>
#include <qwt_date_scale_engine.h>


GpGraph::GpGraph(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GpGraph)
{
    ui->setupUi(this);
}

GpGraph::~GpGraph()
{
    delete ui;
//    delete curv;
}

void GpGraph::resizeEvent(QResizeEvent *e)   // Trap the form resize event to allow the graph to be resized to match.
{                                            // DOESN'T WORK.
    QDialog::resizeEvent(e);
    doResize();
}

void GpGraph::doClose()
{
    this->close();
}

void GpGraph::ggAddData(PlotData *inData)
{
    ggData = inData;
}

void GpGraph::ggLayout()
{
    curv = new QwtPlotCurve();
    QwtPlotGrid *grd = new QwtPlotGrid();
    QwtDateScaleDraw *dsd = new QwtDateScaleDraw(Qt::UTC);
    QwtDateScaleEngine *dse = new QwtDateScaleEngine(Qt::UTC);

    ui->dspTrkName->setText(ggData->trkName);
    ui->dspTrkDate->setText(ggData->trkDate);
    ui->ggPlotArea->setTitle(ggData->trkName);
    ui->ggPlotArea->setCanvasBackground(Qt::white);

    // Label on Y axis.
    ui->ggPlotArea->setAxisTitle(QwtPlot::yLeft, ggData->yLabel);

    if (ggData->xType == "datetime")    // Set up a date scale if x axis contains a time.
    {
        dsd->setDateFormat(QwtDate::Minute, QString("hh:mm"));
        dsd->setDateFormat(QwtDate::Hour, QString("hh:mm"));
        ui->ggPlotArea->setAxisScaleDraw(QwtPlot::xBottom, dsd);
        ui->ggPlotArea->setAxisScaleEngine(QwtPlot::xBottom, dse);
    }

    //      label on X axis.
    QFont fnt("Liberation Sans", 8);
    ui->ggPlotArea->setAxisFont(QwtPlot::xBottom, fnt);
    ui->ggPlotArea->setAxisTitle(QwtPlot::xBottom, ggData->xLabel);
    ui->ggPlotArea->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignRight);
    ui->ggPlotArea->setAxisLabelRotation(QwtPlot::xBottom, 90.0);

    //      Manual scales if requested
    if (ggData->manScale)
    {
        ui->ggPlotArea->setAxisScale(QwtPlot::xBottom, ggData->xLo, ggData->xHi, 0);
        ui->ggPlotArea->setAxisScale(QwtPlot::yLeft, ggData->yLo, ggData->yHi, 0);
    }

    if (ggData->latlon) // If lat/lon plot then force aspect ratio.
    {   // The form design of ggPlotArea has set a size and has set its QSizePolicy as Fixed.
        d_rescaler = new QwtPlotRescaler(ui->ggPlotArea->canvas(),QwtPlot::yLeft,QwtPlotRescaler::Fixed);

        if ( (ggData->xHi - ggData->xLo) < (ggData->yHi - ggData->yLo) )
        {
            d_rescaler->setReferenceAxis(QwtPlot::yLeft);
            d_rescaler->setAspectRatio(QwtPlot::xBottom, 1.0);
        }
        else
        {
            d_rescaler->setReferenceAxis(QwtPlot::xBottom);
            d_rescaler->setAspectRatio(QwtPlot::yLeft, 1.0);
        }
    }
    else    // Allow the plot area to fill the available space.
    {
        ui->ggPlotArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    grd->attach( ui->ggPlotArea);
    curv->setSamples(ggData->xData, ggData->yData);
    curv->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curv->attach(ui->ggPlotArea);
    ui->ggPlotArea->replot();

}

void GpGraph::doResize()
{
    ui->ggPlotArea->replot();
}


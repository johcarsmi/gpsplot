#include "hdr/gpgraph.h"
#include "ui_gpgraph.h"

#include <qwt_date.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_date_scale_draw.h>
#include <qwt_date_scale_engine.h>

#include "hdr/plotdata.h"


GpGraph::GpGraph(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GpGraph)
{
    ui->setupUi(this);
}

GpGraph::~GpGraph()
{
    delete ui;
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
    QwtPlotCurve *curv = new QwtPlotCurve();
    QwtPlotGrid *grd = new QwtPlotGrid();
    QwtDateScaleDraw *dsd = new QwtDateScaleDraw(Qt::UTC);
    QwtDateScaleEngine *dse = new QwtDateScaleEngine(Qt::UTC);

    ui->dspTrkName->setText(ggData->trkName);
    ui->ggPlotArea->setTitle(ggData->trkName);
    ui->ggPlotArea->setCanvasBackground(Qt::white);
    // Label on Y axis.
    ui->ggPlotArea->setAxisTitle(QwtPlot::yLeft, ggData->yLabel);

    // TODO time on X axis.
    dsd->setDateFormat(QwtDate::Minute, QString("hh:mm"));      // Not doing what's wanted at the moment.
    dsd->setDateFormat(QwtDate::Hour, QString("hh:mm"));        // Not doing what's wanted at the moment.
    ui->ggPlotArea->setAxisScaleDraw(QwtPlot::xBottom, dsd);
    ui->ggPlotArea->setAxisScaleEngine(QwtPlot::xBottom, dse);
    //      label on X axis.
    QFont fnt("Liberation Sans", 7);
    ui->ggPlotArea->setAxisFont(QwtPlot::xBottom, fnt);
    ui->ggPlotArea->setAxisTitle(QwtPlot::xBottom, ggData->xLabel);
    ui->ggPlotArea->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignRight);
    ui->ggPlotArea->setAxisLabelRotation(QwtPlot::xBottom, 90.0);

    //qDebug() << ((QwtDateScaleDraw*)ui->ggPlotArea->axisScaleDraw(QwtPlot::xBottom))->dateFormat(QwtDate::Minute);
    // Shows that the plot area has the correct format string.


    //      distance on X axis.

    grd->attach( ui->ggPlotArea);
    curv->setSamples(ggData->xData, ggData->yData);
    curv->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curv->attach(ui->ggPlotArea);
    ui->ggPlotArea->replot();

}

#include "hdr/gpmapplot.h"
#include "hdr/gplatlon.h"

#include <QPainter>

GpMapPlot::GpMapPlot(QWidget *parent) : QWidget(parent)
{
    owner = static_cast<GpLatLon*>(parent); // To allow access to properties of GpLatLon class.
}

GpMapPlot::~GpMapPlot()
{
    // Leave destruction to Qt.
}

void GpMapPlot::paintEvent(QPaintEvent *event)
{
    QPainter paint(this);
    // Paint the background Google satellite image.
    paint.drawImage(0, 0, owner->bgImage);
    // Set up the pen for the track.
    QPen trkPen;
    trkPen.setColor(owner->trkCol);
    trkPen.setWidth(1);
    // Draw the track
    paint.setPen(trkPen);
    paint.drawLines(*owner->trkPlot);
    // Draw the end point offset 1px right.
    trkPen.setColor(Qt::blue);
    paint.setPen(trkPen);
    QBrush trkBr(Qt::blue);
    paint.setBrush(trkBr);
    paint.drawEllipse(owner->trkPlot->at(owner->trkPlot->count() - 1).x() + 1, owner->trkPlot->at(owner->trkPlot->count() - 1).y(), 3, 3);
    // Draw the start point offset 1px left so it overlays end point marker if the two coincide.
    trkPen.setColor(Qt::red);
    paint.setPen(trkPen);
    trkBr.setColor(Qt::red);
    paint.setBrush(trkBr);
    paint.drawEllipse(owner->trkPlot->at(0).x() - 1, owner->trkPlot->at(0).y(), 3, 3);
    QWidget::paintEvent(event);
}

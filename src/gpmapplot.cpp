#include "hdr/gpmapplot.h"
#include "hdr/gplatlon.h"

#include <QPainter>

GpMapPlot::GpMapPlot(QWidget *parent) : QWidget(parent)
{
    owner = static_cast<GpLatLon*>(parent);
}

GpMapPlot::~GpMapPlot()
{

}

void GpMapPlot::paintEvent(QPaintEvent *event)
{
    QPainter paint(this);
    // Paint the background Google satellite image.
    paint.drawImage(0, 0, owner->bgImage);
    // Set up the pen for the track.
    QPen trkPen;
    trkPen.setColor(QColor(0,255,255));
    trkPen.setWidth(1);
    // Draw the track
    paint.setPen(trkPen);
//    paint.setRenderHint(QPainter::Antialiasing, true);
    paint.drawLines(*owner->trkPlot);
    trkPen.setColor(QColor(255,0,0));
    paint.setPen(trkPen);
    QBrush trkBr(QColor(255, 0, 0));
    paint.setBrush(trkBr);
    paint.drawEllipse(owner->trkPlot->at(0).x(), owner->trkPlot->at(0).y(), 3, 3);

    QWidget::paintEvent(event);
}

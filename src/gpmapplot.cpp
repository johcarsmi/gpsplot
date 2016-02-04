#include "hdr/gpmapplot.h"
#include "hdr/gplatlon.h"

#include <QPainter>
#include <QWheelEvent>

GpMapPlot::GpMapPlot(QWidget *parent) : QWidget(parent)
{
    owner = static_cast<GpLatLon*>(parent); // To allow access to properties of GpLatLon class.
    nowPt = new QPoint;
}

GpMapPlot::~GpMapPlot()
{
    delete nowPt;
    // Leave destruction to Qt.
}

void GpMapPlot::paintEvent(QPaintEvent *event)
{
    QPainter paint(this);
    // Paint the background Google satellite image.
    paint.drawImage(0, 0, owner->bgImage);
    // Set up the pen for the track.
    QPen trkPen;
    trkPen.setStyle(Qt::SolidLine);
    trkPen.setColor(owner->trkCol);
    trkPen.setWidth(1);
    // Draw the track.
    paint.setPen(trkPen);
    paint.drawLines(*owner->trkPlot);
    // Draw the arrows.
    trkPen.setColor(owner->arrCol);
    paint.setPen(trkPen);
    doArrows(&paint);
    // Draw the end point offset 1px right.
    trkPen.setColor(Qt::blue);
    paint.setPen(trkPen);
    QBrush trkBr(Qt::blue);
    paint.setBrush(trkBr);
    paint.drawEllipse(owner->trkPlot->at(owner->trkPlot->count() - 1).x() + 1, owner->trkPlot->at(owner->trkPlot->count() - 1).y(), 3, 3);
    // Draw the start point offset 1px left so it overlays end point marker if the two coincide.
    trkBr.setColor(Qt::red);
    paint.setBrush(trkBr);
    paint.drawEllipse(owner->trkPlot->at(0).x() - 1, owner->trkPlot->at(0).y(), 3, 3);
    QWidget::paintEvent(event);
}

void GpMapPlot::doArrows(QPainter * inPaint)
{
    for (int ix =0; ix < owner->arrD->count(); ++ix)
    {
        inPaint->drawLine(owner->arrD->trkPt.at(ix), owner->arrD->lPt.at(ix));
        inPaint->drawLine(owner->arrD->trkPt.at(ix), owner->arrD->rPt.at(ix));
    }
}

void GpMapPlot::mouseMoveEvent(QMouseEvent *event)
{
    int dX, dY;
    dX = event->x() - startX;
    dY = event->y() - startY;
    //qDebug("GpMapPlot: dX = %d : dY = %d", dX, dY);
    nowPt->setX(dX);
    nowPt->setY(dY);
    owner->passDragPos(nowPt);
}

void GpMapPlot::mousePressEvent(QMouseEvent *event)
{
    owner->saveDragStart();
    owner->setPaintBG(false);
    startX = event->x();
    startY = event->y();
}

void GpMapPlot::mouseReleaseEvent(QMouseEvent *event)
{
    endX = event->x();
    endY = event->y();
    owner->setPaintBG(true);
    if (startX == endX && startY == endY) return;
    owner->passDragPos(nowPt);
}

void GpMapPlot::wheelEvent(QWheelEvent *wevent)
{
    int movemt = wevent->delta();
    //qDebug("wheel move %d", movemt);
    if (movemt > 0) owner->doZin();
    if (movemt < 0) owner->doZout();
    wevent->ignore();
}

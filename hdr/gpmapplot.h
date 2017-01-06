#ifndef GPMAPPLOT_H
#define GPMAPPLOT_H

#include <qt5/QtWidgets/QWidget>
#include <qt5/QtGui/QMouseEvent>

#include "hdr/gplatlon.h"

class GpMapPlot : public QWidget
{
    Q_OBJECT
public:
    explicit GpMapPlot(QWidget *parent = 0);
    ~GpMapPlot();

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *wevent);

private:
    GpLatLon *owner;
    int startX;
    int startY;
    int endX;
    int endY;
    QPoint *nowPt;
    void doArrows(QPainter*);
};

#endif // GPMAPPLOT_H

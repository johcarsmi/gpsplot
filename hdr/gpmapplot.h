#ifndef GPMAPPLOT_H
#define GPMAPPLOT_H

#include <QWidget>

#include "hdr/gplatlon.h"

class GpMapPlot : public QWidget
{
    Q_OBJECT
public:
    explicit GpMapPlot(QWidget *parent = 0);
    ~GpMapPlot();

protected:
    void paintEvent(QPaintEvent *event);

private:
    GpLatLon *owner;
    void doArrows(QPainter*);
};

#endif // GPMAPPLOT_H

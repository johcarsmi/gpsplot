#include "hdr/plotdata.h"

PlotData::PlotData()
{
    xLabel = "";
    yLabel = "";
    xType = "";
    yType = "";
    xData.clear();
    yData.clear();
    trkName = "";
    trkDate = "";
    manScale = false;
    xLo = 0.0;
    xHi = 0.0;
    yLo = 0.0;
    yHi = 0.0;

}

ArrowData::ArrowData()
{
    head.clear();
    headB.clear();
    headF.clear();
    dirctn.clear();
}

void ArrowData::appendRow(QPointF a, QPointF b, QPointF c, double d, QPoint e, QPoint f, QPoint g)
{
    head.append(a);
    headB.append(b);
    headF.append(c);
    dirctn.append(d);
    trkPt.append(e);
    lPt.append(f);
    rPt.append(g);
}

int ArrowData::count()
{
    return head.count();
}

void ArrowData::clear()
{
    head.clear();
    headB.clear();
    headF.clear();
    dirctn.clear();
    trkPt.clear();
    lPt.clear();
    rPt.clear();
}

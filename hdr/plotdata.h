#ifndef PLOTDATA_H
#define PLOTDATA_H

#include <qt5/QtCore/QString>
#include <qt5/QtCore/QVector>
#include <qt5/QtCore/QPointF>

class PlotData
{
public:
    PlotData();

    QString xLabel;
    QString yLabel;
    QString xType;
    QString yType;
    QVector <double> xData;
    QVector <double> yData;
    QString trkName;
    QString trkDate;
    bool manScale;
    double xLo;
    double xHi;
    double yLo;
    double yHi;
    bool latlon;
};

class ArrowData
{
public:
    ArrowData();
    void appendRow(QPointF, QPointF, QPointF, double, QPoint, QPoint, QPoint);
    int count();
    void clear();

    QVector <QPointF> head;
    QVector <QPointF> headF;
    QVector <QPointF> headB;
    QVector <double> dirctn;
    QVector <QPoint> trkPt;
    QVector <QPoint> lPt;
    QVector <QPoint> rPt;
};

#endif // PLOTDATA_H

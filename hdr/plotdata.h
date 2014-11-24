#ifndef PLOTDATA_H
#define PLOTDATA_H

#include <QString>
#include <QVector>

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
};

#endif // PLOTDATA_H

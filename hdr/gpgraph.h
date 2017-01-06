#ifndef GPGRAPH_H
#define GPGRAPH_H

#include <qt5/QtWidgets/QDialog>

#include <hdr/plotdata.h>

#include <qwt_plot_curve.h>
#include <qwt_plot_rescaler.h>

namespace Ui {
class GpGraph;
}

class GpGraph : public QDialog
{
    Q_OBJECT

protected:
    void resizeEvent(QResizeEvent *);

public:
    explicit GpGraph(QWidget *parent = 0);
    ~GpGraph();

    void ggAddData (PlotData *);
    void ggLayout();

private:
    Ui::GpGraph *ui;
    PlotData *ggData;
    QwtPlotCurve *curv;
    QwtPlotRescaler *d_rescaler;

    void doResize();

private slots:
    void doClose();

};

#endif // GPGRAPH_H

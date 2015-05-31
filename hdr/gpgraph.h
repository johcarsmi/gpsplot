#ifndef GPGRAPH_H
#define GPGRAPH_H

#include <QDialog>

#include <hdr/plotdata.h>

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
    QwtPlotRescaler *d_rescaler;

private slots:
    void doClose();
    void doResize();

};

#endif // GPGRAPH_H

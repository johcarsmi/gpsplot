#ifndef GPGRAPH_H
#define GPGRAPH_H

#include <QDialog>

#include <hdr/plotdata.h>

namespace Ui {
class GpGraph;
}

class GpGraph : public QDialog
{
    Q_OBJECT

public:
    explicit GpGraph(QWidget *parent = 0);
    ~GpGraph();

    void ggAddData (PlotData *);
    void ggLayout();

private:
    Ui::GpGraph *ui;
    PlotData *ggData;

private slots:
    void doClose();

};

#endif // GPGRAPH_H

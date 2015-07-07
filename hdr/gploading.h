#ifndef GPLOADING_H
#define GPLOADING_H

#include <QWidget>

namespace Ui {
class GpLoading;
}

class GpLoading : public QWidget
{
    Q_OBJECT

public:
    explicit GpLoading(QWidget *parent = 0);
    ~GpLoading();

private:
    Ui::GpLoading *ui;
};

#endif // GPLOADING_H

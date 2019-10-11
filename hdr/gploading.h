#ifndef GPLOADING_H
#define GPLOADING_H

#include <qt5/QtWidgets/QWidget>

namespace Ui {
class GpLoading;
}

class GpLoading : public QWidget
{
    Q_OBJECT

public:
    explicit GpLoading(QWidget *parent);
    ~GpLoading();

private:
    Ui::GpLoading *ui;
};

#endif // GPLOADING_H

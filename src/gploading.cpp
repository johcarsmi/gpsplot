#include "hdr/gploading.h"
#include "ui_gploading.h"

GpLoading::GpLoading(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GpLoading)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
}

GpLoading::~GpLoading()
{
    delete ui;
}

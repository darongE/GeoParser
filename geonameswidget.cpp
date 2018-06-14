#include "geonameswidget.h"
#include "ui_geonameswidget.h"

GeoNamesWidget::GeoNamesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeoNamesWidget)
{
    ui->setupUi(this);
}

GeoNamesWidget::~GeoNamesWidget()
{
    delete ui;
}

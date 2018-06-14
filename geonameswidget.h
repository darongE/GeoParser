#ifndef GEONAMESWIDGET_H
#define GEONAMESWIDGET_H

#include <QWidget>

namespace Ui {
class GeoNamesWidget;
}

class GeoNamesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GeoNamesWidget(QWidget *parent = 0);
    ~GeoNamesWidget();

private:
    Ui::GeoNamesWidget *ui;
};

#endif // GEONAMESWIDGET_H

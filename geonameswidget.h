#ifndef GEONAMESWIDGET_H
#define GEONAMESWIDGET_H

#include <QAbstractItemModel>
#include <QMimeData>
#include <QWidget>
#include "searchbackend.h"

namespace Ui {
class GeoNamesWidget;
}


class GeoNamesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GeoNamesWidget(QWidget *parent = 0);
    ~GeoNamesWidget();

private slots:

    void slotSearchCompleted();
    void slotTriggerSearch();
    void slotCurrentlySelectedResultChanged(const QModelIndex& current
                                            ,const QModelIndex& previous );
    void slotUpdateAction();
    void slotRemoveSelectedFromResultList();
    void slotClearSearchResults();

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::GeoNamesWidget *ui;

    class Private;
    Private* const d;
};

#endif // GEONAMESWIDGET_H

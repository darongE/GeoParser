#include "geonameswidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GeoNamesWidget w;
    w.show();

    return a.exec();
}

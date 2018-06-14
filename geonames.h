#ifndef GEONAMES_H
#define GEONAMES_H

#include <QObject>

class GeoNames : public QObject
{
    Q_OBJECT
public:

    QString name;
    QString country;
    double latitude;
    double longitude;
    int      elevation;
    QString  timezone;

    explicit GeoNames(QObject *parent = nullptr);


    GeoNames(const GeoNames& other);
    GeoNames& operator = (const GeoNames& other);
    bool isValid() const  { return name.isEmpty(); }

signals:

public slots:
};

#endif // GEONAMES_H



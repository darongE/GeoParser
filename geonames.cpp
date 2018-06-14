#include "geonames.h"
#include <QString>

GeoNames::GeoNames(QObject *parent) : QObject(parent)
  ,name("")
  ,country("")
  ,latitude(0)
  ,longitude(0)
  ,elevation(0)
  ,timezone("")
{

}

GeoNames::GeoNames(const GeoNames &other)
    : QObject(),
      name(other.name),
      country(other.country),
      latitude(other.latitude),
      longitude(other.longitude),
      elevation(other.elevation),
      timezone(other.timezone)

{

}


GeoNames& GeoNames::operator = (const GeoNames& other)
{
    name = other.name;
    country = other.country;
    latitude = other.latitude;
    longitude = other.longitude;
    elevation = other.elevation;
    timezone = other.timezone;
    return *this;
}

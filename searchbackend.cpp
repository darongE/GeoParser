#include "searchbackend.h"

#include <QNetworkAccessManager>
#include <QDebug>
#include <QDomDocument>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>


class SearchBackend::Private
{
public:

    explicit Private()
        :results(),
         netReply(0),
         runningBackend(),
         searchData(),
         errorMessage()

    {

    }

    SearchBackend::SearchResult::List results;
    QNetworkReply*                    netReply;
    QString                           runningBackend;
    QByteArray                        searchData;
    QString                           errorMessage;

};

SearchBackend::SearchBackend(QObject * const parent)
    : QObject(parent),
      d(new Private())
{

}

SearchBackend::~SearchBackend()
{
    delete d;
}

bool SearchBackend::search(const QString &backendName, const QString &searchTerm)
{
    d->searchData.clear();
    d->errorMessage.clear();
    d->results.clear();

    QNetworkAccessManager* const mngr = new QNetworkAccessManager(this);

    connect(mngr, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(slotFinished(QNetworkReply*)));

    if(backendName == QLatin1String("osm"))
    {
        d->runningBackend = backendName;

        QUrl netUrl(QLatin1String("http://nominatim.openstreetmap.org/search"));
        //https://nominatim.openstreetmap.org/?q=seoul&format=xml&addressdetails=1&limit=1&accept-language=en


        QUrlQuery q(netUrl);
        q.addQueryItem(QLatin1String("q"), searchTerm);
        q.addQueryItem(QLatin1String("format"), QLatin1String("xml"));
        q.addQueryItem(QLatin1String("addressdetails"), QLatin1String("1"));
        q.addQueryItem(QLatin1String("limit"), QLatin1String("1"));
        q.addQueryItem(QLatin1String("accept-language"), QLatin1String("en"));

        netUrl.setQuery(q);

        qDebug() << netUrl;

        QNetworkRequest netRequest(netUrl);
        netRequest.setRawHeader("User-Agent", "Mozilla");
        netRequest.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                                QNetworkRequest::NoLessSafeRedirectPolicy);
        d->netReply = mngr->get(netRequest);

        return true;
    }

    if(backendName == QLatin1String("geonames.org"))
    {
        d->runningBackend = backendName;

        // documentation: http://www.geonames.org/export/geonames-search.html
           // http://api.geonames.org/search?q=seoul&maxRows=10&featureClass=A&featureClass=P&orderby=relevance&username=yoshimi

        //http://api.geonames.org/wikipediaSearch?q=london&maxRows=1&username=yoshimi
          QUrl netUrl(QLatin1String("http://api.geonames.org/searchJSON"));



          QUrlQuery q(netUrl);
      //    q.addQueryItem(QLatin1String("type"), QLatin1String("xml"));
          q.addQueryItem(QLatin1String("q"), searchTerm);
          q.addQueryItem(QLatin1String("maxRows"),QLatin1String("1"));
          q.addQueryItem(QLatin1String("orderby"),QLatin1String("relevance"));
          q.addQueryItem(QLatin1String("username"), QLatin1String("yoshimi"));

          netUrl.setQuery(q);

          QNetworkRequest netRequest(netUrl);
          //"User-Agent", "Mozilla"
          netRequest.setRawHeader("User-Agent","Mozilla");
          netRequest.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                                  QNetworkRequest::NoLessSafeRedirectPolicy);

          d->netReply = mngr->get(netRequest);


          return true;
     }

   return false;
 }





 void SearchBackend::slotFinished(QNetworkReply* reply)
 {
     if (reply != d->netReply)
     {
         return;
     }

     if (reply->error() != QNetworkReply::NoError)
     {
         d->errorMessage = reply->errorString();
         emit(signalSearchCompleted());
         reply->deleteLater();
         return;
     }

    d->searchData.append(reply->readAll());

    const QString resultString = QString::fromUtf8(d->searchData.constData(), d->searchData.count());


    qDebug() << "data" << resultString;

    if(d->runningBackend == QLatin1String("osm"))
    {
        QDomDocument doc;
        doc.setContent(resultString);

        QDomElement docElement = doc.documentElement();

        for(QDomNode resultNode = docElement.firstChild(); !resultNode.isNull(); resultNode = resultNode.nextSibling())
        {
            QDomElement resultElement = resultNode.toElement();

            if(resultElement.isNull())
                continue;


            if(resultElement.tagName() != QLatin1String("place"))
                continue;

            const QString boundingBoxString = resultElement.attribute(QLatin1String("boundingbox"));
            const QString latString         = resultElement.attribute(QLatin1String("lat"));
            const QString lonString         = resultElement.attribute(QLatin1String("lon"));
            const QString displyaName       = resultElement.attribute(QLatin1String("display_name"));
            const QString placeId           = resultElement.attribute(QLatin1String("place_id"));


            qDebug() << "BoundingBox" << boundingBoxString
                     << "Latitude   " << latString
                     << "Longitude   " << lonString
                      << "Longitude   "<< displyaName
                      << "Place Id"   << placeId;




            if(latString.isEmpty() || lonString.isEmpty() || displyaName.isEmpty())
            {
                continue;
            }

            //parse the string
            qreal lat, lon;

            bool okay = false;
            lat = latString.toDouble(&okay);

            if(okay)
                lon = lonString.toDouble(&okay);

            if(!okay)
                continue;

            SearchResult  result;
            result.coordinates = GeoCoordinates(lat, lon);
            result.name        = displyaName;

            if(!placeId.isEmpty())
                result.internalId = QLatin1String("osm-") + placeId;

            d->results << result;

        }

    }
    else if(d->runningBackend == QLatin1String("geonames.org") )
    {

        //url
        // http://api.geonames.org/search?q=seoul&maxRows=10&featureClass=A&featureClass=P&orderby=relevance&username=yoshimi

        QJsonDocument doc = QJsonDocument::fromJson(resultString.toUtf8());

         if(doc.isNull()) {
               qDebug() << "Doc is null/invalid in inverts.";
             return;
         }


        QJsonObject responseObject = doc.object();
        QJsonArray dataObject = responseObject["geonames"].toArray();
        QJsonObject geonamesObj = dataObject.at(0).toObject();

        QString latString;
        QString lonString;
        QString displayName;
        QString countryName;
        int geoNameId;

        latString = geonamesObj["lat"].toString();
        lonString = geonamesObj["lng"].toString();
        displayName =geonamesObj["name"].toString();
        countryName = geonamesObj["countryName"].toString();
        geoNameId   = geonamesObj["geonameId"].toInt();

        // now parse the strings:
        qreal lat;
        qreal lon;
        lat = latString.toDouble();
        lon = lonString.toDouble();

        qDebug() << "Lat," << "Long" << lat << lon;

        SearchResult m_result;
        m_result.coordinates = GeoCoordinates(lat, lon);
        m_result.name        = displayName;
        m_result.country     = countryName;

        QString strgeoNameId = QString::number(geoNameId);
        if (!strgeoNameId.isEmpty())
        {
            m_result.internalId = QLatin1String("geonames.org-") + strgeoNameId;
        }


        d->results << m_result;


    }

    emit signalSearchCompleted();

    reply->deleteLater();
}

 SearchBackend::SearchResult::List SearchBackend::getResults() const
 {
     return d->results;
 }


QString SearchBackend::getErrorMessage() const
{
    return d->errorMessage;
}
















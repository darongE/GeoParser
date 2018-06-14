#include "searchbackend.h"

#include <QNetworkAccessManager>
#include <QDomDocument>
#include <QUrlQuery>


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

        QUrlQuery q(netUrl);
        q.addQueryItem(QLatin1String("format"), QLatin1String("xml"));
        q.addQueryItem(QLatin1String("q"), searchTerm);
        netUrl.setQuery(q);

        QNetworkRequest netRequest(netUrl);

        d->netReply = mngr->get(netRequest);

        return true;
    }

    if(backendName == QLatin1String("geonames.org"))
    {
        d->runningBackend = backendName;

        // documentation: http://www.geonames.org/export/geonames-search.html

          QUrl netUrl(QLatin1String("http://api.geonames.org/search"));

          QUrlQuery q(netUrl);
          q.addQueryItem(QLatin1String("type"), QLatin1String("xml"));
          q.addQueryItem(QLatin1String("q"), searchTerm);
          q.addQueryItem(QLatin1String("username"), QLatin1String("digikam"));
          netUrl.setQuery(q);

          QNetworkRequest netRequest(netUrl);

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




}

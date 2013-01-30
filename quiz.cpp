#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStringList>
#include <QXmlStreamReader>
#include <QDebug>
#include "quiz.h"

struct MediaWikiPrivate {
    QUrl apiUrl;
    QNetworkAccessManager *manager;
    int itemsPerSearch;
    int resultTotalItems;
    int resultOffset;
};

MediaWiki::MediaWiki( MainWindow *parent )
        : QObject( parent )
{
    mainWindow = parent;
    m_requestType = REQUEST_NONE;
    d = new MediaWikiPrivate;
    d->resultTotalItems = 0;
    d->resultOffset = 0;
    QString url = QString("http://")+mainWindow->localeString+QString(".wikipedia.org/w/api.php");
    d->apiUrl = QUrl(url);
    d->manager = new QNetworkAccessManager( this );
    d->itemsPerSearch = 50;
    browser = new Browser(0);
    browser->setVisible(false);
    connect( d->manager, SIGNAL(finished(QNetworkReply*)), SLOT(finished(QNetworkReply *)) );
    connect( mainWindow->editLocale, SIGNAL(currentIndexChanged(int)), browser,SLOT(setWikiUrl(int)) );
}

MediaWiki::~MediaWiki()
{
    delete d;
}


int MediaWiki::itemsPerSearch() const
{
    return d->itemsPerSearch;
}

void MediaWiki::setitemsPerSearch( int limit )
{
    d->itemsPerSearch = limit;
}

QUrl MediaWiki::apiUrl() const
{
    return d->apiUrl;
}

void MediaWiki::setApiUrl( const QUrl &url )
{
    d->apiUrl = url;
}


void MediaWiki::search(  )
{
    QString searchTerm = mainWindow->edit->lineEdit()->text();
    m_requestType = REQUEST_SEARCH;

      QUrl url = d->apiUrl;
      url.addQueryItem( QString("action"), QString("query") );
      url.addQueryItem( QString("format"), QString("xml") );
      url.addQueryItem( QString("list"), QString("search") );
      url.addQueryItem( QString("srsearch"), searchTerm );
      url.addQueryItem( QString("srlimit"), QString::number(d->itemsPerSearch) );
      url.addQueryItem( QString("sroffset"), QString::number( d->resultOffset) );
      qDebug() << "Constructed search URL" << url;
      d->manager->get( QNetworkRequest(url) );
}

void MediaWiki::showTitle( const QString &searchTerm )
{
    QUrl url = d->apiUrl;
    url.addQueryItem( QString("action"), QString("query") );
    url.addQueryItem( QString("format"), QString("xml") );
    url.addQueryItem( QString("titles"), searchTerm );
    url.addQueryItem( QString("prop"), QString("revisions") );
    url.addQueryItem( QString("rvprop"), QString("content") );

    qDebug() << "Constructed search URL" << url;

    m_requestType = REQUEST_TITLE_CONTENT;
    d->manager->get( QNetworkRequest(url) );
}

void MediaWiki::finished( QNetworkReply *reply )
{
    if ( reply->error() != QNetworkReply::NoError ) {
        qDebug() << "Request failed, " << reply->errorString();
        reply->deleteLater();
        return;
    }

    qDebug() << "Request succeeded";
    bool ok = false;
    if( m_requestType == REQUEST_TITLE_CONTENT) {
        ok = processTitleResult( reply );
        reply->deleteLater();
        m_requestType = REQUEST_SEARCH;
    } else if(m_requestType == REQUEST_SEARCH) {
        ok = processSearchResult( reply );
        d->resultOffset+=d->itemsPerSearch;
        reply->deleteLater();
        qDebug() << "resultOffset = " << d->resultOffset;
        qDebug() << "resultItemsTotal = " << d->resultTotalItems;
        if(d->resultOffset < d->resultTotalItems ) {
            QTimer::singleShot(0,this,SLOT(search()));
        } else {
            d->resultOffset = 0;
            d->resultTotalItems = 0;
        }
    }
}

void MediaWiki::initSearch()
{
    mainWindow->list->clear();
    d->resultOffset = 0;
    d->resultTotalItems = 0;
}

void MediaWiki::itemBrowse( QListWidgetItem *item )
{
    browser->lineEdit->setText(item->text());
    browser->setVisible(true);
    browser->loadWikiPage();
    showTitle(item->text());
}

bool MediaWiki::processTitleResult( QIODevice *source )
{
    QXmlStreamReader reader( source );
    while ( !reader.atEnd() ) {
        QXmlStreamReader::TokenType tokenType = reader.readNext();
        qDebug() << "Token" << int(tokenType);
        if ( tokenType == QXmlStreamReader::Characters ) {
        qDebug() << "data" << int(reader.isCDATA());
                QXmlStreamAttributes attrs = reader.attributes();
                qDebug() << reader.text().toString();
        }
        else if ( tokenType == QXmlStreamReader::Invalid )
            return false;
    }
    return true;
}

bool MediaWiki::processSearchResult( QIODevice *source )
{
    QXmlStreamReader reader( source );
    while ( !reader.atEnd() ) {
        QXmlStreamReader::TokenType tokenType = reader.readNext();
        // qDebug() << "Token" << int(tokenType);
        if ( tokenType == QXmlStreamReader::StartElement ) {
            if ( reader.name() == QString("searchinfo") ) {
                QXmlStreamAttributes attrs = reader.attributes();
                d->resultTotalItems = attrs.value( QString("totalhits") ).toString().toInt();
                qDebug() << "resultsTotalItems" << d->resultTotalItems;
            }

            if ( reader.name() == QString("p") ) {
                QXmlStreamAttributes attrs = reader.attributes();
                qDebug() << "Found page" << attrs.value( QString("title") );
                mainWindow->list->addItem(attrs.value( QString("title") ).toString());

            }
        }
        else if ( tokenType == QXmlStreamReader::Invalid )
            return false;
    }
    return true;
}

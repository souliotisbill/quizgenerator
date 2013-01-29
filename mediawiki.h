#ifndef MEDIAWIKI_H
#define MEDIAWIKI_H

#include <QtCore>
#include <QtGui>
#include <QObject>
#include <QStringList>
#include <QUrl>
#include <QtWebKit/QWebView>
#include "mainwindow.h"

class QNetworkReply;

enum {
    REQUEST_NONE,
    REQUEST_SEARCH,
    REQUEST_TITLE_CONTENT,
    REQUEST_MAX
};

class Browser : public QMainWindow
{
  Q_OBJECT

  public:
    Browser(QWidget *p);
    virtual ~Browser();
    QLineEdit *lineEdit;
  public slots:
    void loadWikiPage();
    void setWikiUrl(int index);

  private slots:
    void addToCombo();

  private:
    QPushButton *back;
    QPushButton *forward;
    QPushButton *reload;
    QComboBox *historyCombo;
    QLabel *historyLabel;
    QWebView *webView;
    QWidget *menuWidget;
    QStatusBar *statusBar;
    QProgressBar *progressBar;
//    QLocale *locale;
    QString countryCode;
    QString wikiUrl;
};


class MediaWiki : public QObject
{
    Q_OBJECT

public:
    MediaWiki( MainWindow *parent );
    virtual ~MediaWiki();
    Browser *browser;
    MainWindow *mainWindow;
    unsigned int m_requestType;

    int itemsPerSearch() const;
    void setitemsPerSearch( int limit );

    QUrl apiUrl() const;
    void setApiUrl( const QUrl &url );

signals:

public slots:
    void initSearch();
    void search(  );
    void showTitle( const QString &searchTerm );
    void itemBrowse( QListWidgetItem *item );

private slots:
    void finished( QNetworkReply *reply );

private:
    bool processSearchResult( QIODevice *source );
    bool processTitleResult( QIODevice *source );

    struct MediaWikiPrivate *d;
};

#endif // MEDIAWIKI_H

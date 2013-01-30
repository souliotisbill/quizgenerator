#include <QLineEdit>
#include <QListWidget>
#include <QVBoxLayout>

#include "mainwindow.h"
#include "quiz.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    layoutV = new QVBoxLayout(this);
    localeString = QString("el");
    layoutH = new QHBoxLayout();

    edit = new QComboBox();
    editLocale->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    edit->setEditable(true);

    editLocale = new QComboBox(  );
    editLocale->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    editLocale->addItem(QIcon(QString("./Greece-Flag-icon.png")),QString("Gr"));
    editLocale->addItem(QIcon(QString("./English-Language-Flag-1-icon.png")),QString("En"));
    editLocale->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    editLocale->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    layoutH->addWidget( edit );
    layoutH->addWidget( editLocale );
    list = new QListWidget;

    layoutV->addLayout( layoutH );
    layoutV->addWidget( list );

//    delete layout();
//    setLayout(layoutV);

    connect( editLocale, SIGNAL(currentIndexChanged(int)), SLOT(changeLocale(int)) );
//    connect( edit, SIGNAL(currentIndexChanged(QString )), SLOT(go()) );
    connect( edit->lineEdit(), SIGNAL(returnPressed()), SLOT(go()) );

    wiki = new MediaWiki( this );
    wiki->setitemsPerSearch(50);



    connect( list, SIGNAL(itemDoubleClicked(QListWidgetItem *)),wiki, SLOT(itemBrowse(QListWidgetItem *)) );
}

MainWindow::~MainWindow()
{
}

void MainWindow::changeLocale(int index)
{
    qDebug() << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << index;
    if(index == 0) localeString = QString("el");
    else localeString = QString("en");
    QString url = QString("http://")+localeString+QString(".wikipedia.org/w/api.php");
    qDebug() << url;
    wiki->setApiUrl( QUrl(url) );
}

void MainWindow::go()
{
    wiki->initSearch();
    wiki->search(  );
}

Browser::Browser(QWidget *par)
    : QMainWindow(par)
{
//  locale = new QLocale();
//  wikiUrl = "http://" + locale->name().split('_').at(0) + ".wikipedia.org";
  wikiUrl = QString("http://") + "el" + QString(".wikipedia.org");

  menuWidget = new QWidget();
  QHBoxLayout *lay = new QHBoxLayout(menuWidget);

  back = new QPushButton(tr("&Back"));
  forward = new QPushButton(tr("&Forward"));
  reload = new QPushButton(tr("&Reload"));

  historyCombo = new QComboBox();
  historyCombo->setMaxVisibleItems(20);
  historyCombo->setMinimumWidth(150);
  historyLabel = new QLabel(QObject::tr("History:"));

  lineEdit = new QLineEdit();

  lay->addWidget(back);
  lay->addWidget(lineEdit);
  lay->addWidget(historyLabel);
  lay->addWidget(historyCombo);
  lay->addWidget(forward);
  lay->addWidget(reload);
  setMenuWidget(menuWidget);

  webView = new QWebView();
  setCentralWidget(webView);
  webView->load(QUrl(wikiUrl));
  webView->setTextSizeMultiplier(1.0);

  progressBar = new QProgressBar();
  statusBar = new QStatusBar();
  statusBar->addWidget(progressBar);
  setStatusBar(statusBar);

  connect(back, SIGNAL(clicked()), webView, SLOT(back()));
  connect(forward, SIGNAL(clicked()), webView, SLOT(forward()));
  connect(reload, SIGNAL(clicked()), webView, SLOT(reload()));
  connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(addToCombo()));
  connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(loadWikiPage()));
  connect(historyCombo, SIGNAL(activated(const QString)), lineEdit, SLOT(setText(const QString)));
  connect(historyCombo, SIGNAL(activated(const QString)), this, SLOT(loadWikiPage()));
  connect(webView, SIGNAL(loadProgress(int)), progressBar, SLOT(setValue(int)));
}

Browser::~Browser()
{
}

void Browser::setWikiUrl(int index)
{
    QString localeString;
    if(index == 0) localeString = QString("el");
    else localeString = QString("en");
    wikiUrl = QString("http://") + localeString + QString(".wikipedia.org");
}

void Browser::addToCombo()
{
  historyCombo->insertItem(0, lineEdit->text());
}

void Browser::loadWikiPage()
{
  QString urlString = wikiUrl + "/wiki/" + lineEdit->text().replace(' ', '_');
  QUrl url = urlString;

  qDebug() << url;

  webView->load(url);
}

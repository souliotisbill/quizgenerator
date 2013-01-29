#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

class MediaWiki;
class QLineEdit;
class QListWidget;
class QVBoxLayout;
class QHBoxLayout;
class QComboBox;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
QComboBox *edit;
QComboBox *editLocale;
QListWidget *list;
QString localeString;
public slots:
    void go();
    void changeLocale(int);
private slots:

private:
    MediaWiki *wiki;

    QVBoxLayout *layoutV;
    QHBoxLayout *layoutH;
};

#endif // MAINWINDOW_H

#ifndef GENMAINWINDOW_H
#define GENMAINWINDOW_H

#include "rawbmpwidget.h"

#include <QMainWindow>
#include <QVector>
#include <QKeyEvent>
#include <QPushButton>
#include <QLineEdit>

class MapModule
{
public:
    MapModule(){}
    MapModule(RawBmpWidget* rbw, QString fileName, int row, int col, float scale){
        this->rbw = rbw;
        this->fileName = fileName;
        this->row = row;
        this->col = col;
        this->scale = scale;

    }
    RawBmpWidget* rbw;
    QString fileName;
    int row;
    int col;
    float scale;
};

class BatMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit BatMainWindow(QWidget *parent = 0);

    QVector<MapModule> mms;

    int loadMapDescr(QString path);

    int setupLayout();

    int loadMaps();  // Only the directory input

    int cleanMaps();

    int createFolderList(QString root);

private:
    QString startFolder;
    QStringList folderList;
    int folderIdx;

    QPushButton *pbGoAhead, *pbGoBack;
    QLineEdit *leFolder;

private slots:
    void onPBGoAhead();
    void onPBGoBack();

protected:
//    void keyPressEvent(QKeyEvent *ke);

signals:

public slots:
};

#endif // GENMAINWINDOW_H

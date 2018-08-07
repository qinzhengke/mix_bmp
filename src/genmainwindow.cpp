#include "genmainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QGridlayout>
#include <QDir>

BatMainWindow::BatMainWindow(QWidget *parent) : QMainWindow(parent)
{
    setFocusPolicy(Qt::StrongFocus);
}

int BatMainWindow::loadMapDescr(QString path)
{
    QFile qf(path);
    if (!qf.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        MSG_REPORT(tr("Cannot open file:")+path);
        return -1;
    }

    QTextStream ts(&qf);
    while(!ts.atEnd())
    {
        QString line = ts.readLine();
        QStringList words = line.split(" ");
        if(4 != words.size())
        {
            MSG_REPORT(tr("Error in config file: ") + line);
            return -1;
        }

        mms.append(MapModule(new RawBmpWidget(), words[0],words[1].toInt(),
                words[2].toInt(), words[3].toFloat()));
    }
    qf.close();

    setupLayout();
    startFolder = path;

    return 0;

}

int BatMainWindow::setupLayout()
{
    QWidget *centerWidget = new QWidget(this);
    QGridLayout * centerLayout = new QGridLayout();
    for(int i=0; i<mms.size(); i++)
    {
        MapModule mm = mms.at(i);
        centerLayout->addWidget(mm.rbw, mm.row, mm.col, 1, 1);
    }
    pbGoAhead = new QPushButton(tr("Ahead"),this);
    pbGoBack  = new QPushButton(tr("Back"),this);
    leFolder = new QLineEdit(this);
    centerLayout->addWidget(pbGoAhead, mms.back().row+1, 1, 1, 1);
    centerLayout->addWidget(pbGoBack, mms.back().row+1, 0, 1, 1);
    centerLayout->addWidget(leFolder, mms.back().row+1, 2, 1, 1);
    centerWidget->setLayout(centerLayout);
    setCentralWidget(centerWidget);
    update();

    connect(pbGoAhead, SIGNAL(clicked(bool)), this, SLOT(onPBGoAhead()));
    connect(pbGoBack, SIGNAL(clicked(bool)), this, SLOT(onPBGoBack()));

    return 0;
}

int BatMainWindow::loadMaps()
{
    QString dir = folderList.at(folderIdx);
    leFolder->setText(dir);
    for(int i=0; i<mms.size(); i++)
    {
        QString path = dir + QDir::separator() + mms.at(i).fileName;
        mms.at(i).rbw->open(path.toStdString());
    }
    return 0;
}

int BatMainWindow::cleanMaps()
{
    for(int i=0; i<mms.size(); i++)
    {
        mms.at(i).rbw->clean();
    }
    return 0;
}

int BatMainWindow::createFolderList(QString root)
{
    QDir dir(root);
    QFileInfoList list = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
    for(int i=0; i<list.size(); i++)
    {
        QFileInfo info = list.at(i);
        if(info.isDir())
        {
            folderList.append(info.absoluteFilePath());
        }
    }
    folderIdx = 0;
    return 0;
}

void BatMainWindow::onPBGoAhead()
{
    if(folderIdx < folderList.size()-1)
    {
        folderIdx ++ ;
        loadMaps();
    }
}

void BatMainWindow::onPBGoBack()
{
    if(folderIdx>0)
    {
        folderIdx --;
        loadMaps();
    }
}
//void BatMainWindow::keyPressEvent(QKeyEvent *ke)
//{
//    int key = ke->key();
//    if(75 == key)   // Left arrow
//    {
//        if(folderIdx>0)
//        {
//            folderIdx --;
//            loadMaps();
//        }
//    }
//    else if(77 == key)
//    {
//        if(folderIdx < folderList.size()-1)
//        {
//            folderIdx ++ ;
//            loadMaps();
//        }
//    }
//}

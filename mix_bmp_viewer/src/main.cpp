#include "rawbmpwidget.h"
#include "genmainwindow.h"
#include <QApplication>
#include <QDir>

#include <iostream>

using namespace std;

void print_hex(string str)
{
    cout<<str<<" = { ";
    for(int i=0; i<str.size(); i++)
    {
        cout<<std::hex<<str.at(i)<<", ";
    }
    cout<<" }"<<endl;
}

void print_hex(char *str)
{
    int N = strlen(str);
    int i = 0;
    printf("len= %d,  \"%s\"= { ", N, str);
    for(i=0; i<N; i++)
    {
        printf("%x, ", str[i]);
    }
    printf("}\n");
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    RawBmpWidget rbw;
//    rbw.show();


    int st = 0;
    if(2 <= argc)
    {
        QString path = QString(argv[1]);
        if(QDir(path).exists()) // batch mode.
        {
            BatMainWindow bmw;
            bmw.loadMapDescr("map_descr.txt");
            bmw.createFolderList(path);
            bmw.show();
            bmw.loadMaps();
            st = a.exec();
        }
        else
        {
            MainWindow mw;
            mw.show();
            mw.rbwMainMap->open(path);
            st = a.exec();
        }
    }

    return st;
}

#include "rawbmpwidget.h"
#include <QApplication>

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
    MainWindow mw;
    mw.show();

    if(argc >= 2);
    {
        mw.rbwMainMap->open(string(argv[1]));
    }

    return a.exec();
}
